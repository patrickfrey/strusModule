/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "storageObjectBuilder.hpp"
#include "strus/lib/queryproc.hpp"
#include "strus/lib/queryproc.hpp"
#include "strus/lib/database_leveldb.hpp"
#include "strus/lib/storage.hpp"
#include "strus/lib/queryeval.hpp"
#include "strus/lib/statsproc.hpp"
#include "strus/storageModule.hpp"
#include "strus/databaseInterface.hpp"
#include "strus/databaseClientInterface.hpp"
#include "strus/storageInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/postingJoinOperatorInterface.hpp"
#include "strus/weightingFunctionInterface.hpp"
#include "strus/summarizerFunctionInterface.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/configParser.hpp"
#include "utils.hpp"
#include "errorUtils.hpp"
#include "internationalization.hpp"
#include <string>
#include <cstring>
#include <memory>

using namespace strus;

StorageObjectBuilder::StorageObjectBuilder( ErrorBufferInterface* errorhnd_)
	:m_queryProcessor( strus::createQueryProcessor(errorhnd_))
	,m_storage(strus::createStorage(errorhnd_))
	,m_statsprocmap()
	,m_errorhnd(errorhnd_)
{
	if (!m_queryProcessor.get()) throw strus::runtime_error(_TXT("error creating '%s'"), "query processor");
	if (!m_storage.get()) throw strus::runtime_error(_TXT("error creating '%s'"), "storage");

	DatabaseReference dbref( strus::createDatabase_leveldb( m_errorhnd));
	if (!dbref.get()) throw strus::runtime_error( _TXT( "failed to create handle for default key value store database '%s'"), "leveldb");
	m_dbmap[ "leveldb"] = dbref;
	m_dbmap[ ""] = dbref;

	StatisticsProcessorReference spref( strus::createStatisticsProcessor( m_errorhnd));
	if (!spref.get()) throw strus::runtime_error( _TXT( "failed to create handle for default statistics processor"));
	m_statsprocmap[ "default"] = spref;
	m_statsprocmap[ ""] = spref;
}

const QueryProcessorInterface* StorageObjectBuilder::getQueryProcessor() const
{
	return m_queryProcessor.get();
}

void StorageObjectBuilder::addStorageModule( const StorageModule* mod)
{
	if (m_errorhnd->hasError())
	{
		m_errorhnd->report(_TXT("cannot add storage module with previous unhandled errors"));
		return;
	}
	if (mod->postingIteratorJoinConstructor)
	{
		PostingIteratorJoinConstructor const* pi = mod->postingIteratorJoinConstructor;
		for (; pi->create != 0; ++pi)
		{
			PostingJoinOperatorInterface* func = pi->create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->report(_TXT("error creating posting join operator"));
				return;
			}
			else
			{
				m_queryProcessor->definePostingJoinOperator( pi->name, func);
				if (m_errorhnd->hasError())
				{
					delete func;
					m_errorhnd->report(_TXT("error defining posting join operator"));
					return;
				}
			}
		}
	}
	if (mod->weightingFunctionConstructor)
	{
		WeightingFunctionConstructor const* wi = mod->weightingFunctionConstructor;
		for (; wi->create != 0; ++wi)
		{
			WeightingFunctionInterface* func = wi->create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->report(_TXT("error creating weighting function"));
				return;
			}
			m_queryProcessor->defineWeightingFunction( wi->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report(_TXT("error defining weighting function"));
				return;
			}
		}
	}
	if (mod->summarizerFunctionConstructor)
	{
		SummarizerFunctionConstructor const* si = mod->summarizerFunctionConstructor;
		for (; si->create != 0; ++si)
		{
			SummarizerFunctionInterface* func = si->create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->report(_TXT("error creating summarizer function '%s'"), si->name);
				return;
			}
			m_queryProcessor->defineSummarizerFunction( si->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report(_TXT("error defining summarizer function '%s'"), si->name);
				return;
			}
		}
	}
	if (mod->scalarFunctionParserConstructor)
	{
		ScalarFunctionParserConstructor const* si = mod->scalarFunctionParserConstructor;
		for (; si->create != 0; ++si)
		{
			ScalarFunctionParserInterface* func = si->create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->report(_TXT("error creating scalar function parser '%s'"), si->name);
				return;
			}
			m_queryProcessor->defineScalarFunctionParser( si->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report(_TXT("error defining scalar function parser '%s'"), si->name);
				return;
			}
		}
	}
	try
	{
		m_storageModules.push_back( mod);

		if (mod->databaseReference.create && mod->databaseReference.name)
		{
			DatabaseReference dbref( mod->databaseReference.create( m_errorhnd));
			if (!dbref.get()) throw strus::runtime_error( _TXT( "failed to create data base reference loaded from module: '%s': %s"), mod->databaseReference.name, m_errorhnd->fetchError());
			m_dbmap[ utils::tolower( mod->databaseReference.name)] = dbref;
		}
		if (mod->statisticsProcessorReference.create && mod->statisticsProcessorReference.name)
		{
			StatisticsProcessorReference spref( mod->statisticsProcessorReference.create( m_errorhnd));
			if (!spref.get()) throw strus::runtime_error( _TXT( "failed to create statistics processor reference loaded from module: '%s': %s"), mod->statisticsProcessorReference.name, m_errorhnd->fetchError());
			m_statsprocmap[ utils::tolower( mod->statisticsProcessorReference.name)] = spref;
		}
	}
	catch (const std::runtime_error& err)
	{
		m_errorhnd->report(_TXT("failed to add storage module: %s"), err.what());
	}
	catch (const std::bad_alloc&)
	{
		m_errorhnd->report(_TXT("out of memory adding storage module"));
	}
}

const DatabaseInterface* StorageObjectBuilder::getDatabase( const std::string& config) const
{
	try
	{
		std::string configstr( config);
		std::string name;
		(void)strus::extractStringFromConfigString( name, configstr, "database", m_errorhnd);
		if (m_errorhnd->hasError())
		{
			m_errorhnd->explain(_TXT("cannot evaluate database: %s"));
			return 0;
		}
		std::map<std::string,DatabaseReference>::const_iterator
			di = m_dbmap.find( utils::tolower( name));
		if (di == m_dbmap.end())
		{
			throw strus::runtime_error( _TXT( "undefined key value store database '%s'"), name.c_str());
		}
		else
		{
			return di->second.get();
		}
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error getting database from storage object builder: %s"), *m_errorhnd, 0);
}

const StatisticsProcessorInterface* StorageObjectBuilder::getStatisticsProcessor( const std::string& name) const
{
	try
	{
		std::map<std::string,StatisticsProcessorReference>::const_iterator
			si = m_statsprocmap.find( utils::tolower( name));
		if (si == m_statsprocmap.end())
		{
			throw strus::runtime_error( _TXT( "undefined statistics processor '%s'"), name.c_str());
		}
		else
		{
			return si->second.get();
		}
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error getting statistics processor from storage object builder: %s"), *m_errorhnd, 0);
}

const StorageInterface* StorageObjectBuilder::getStorage() const
{
	return m_storage.get();
}

QueryEvalInterface* StorageObjectBuilder::createQueryEval() const
{
	return strus::createQueryEval( m_errorhnd);
}


