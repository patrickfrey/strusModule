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
#include "strus/lib/sentence.hpp"
#include "strus/storageModule.hpp"
#include "strus/databaseInterface.hpp"
#include "strus/databaseClientInterface.hpp"
#include "strus/storageInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/fileLocatorInterface.hpp"
#include "strus/postingJoinOperatorInterface.hpp"
#include "strus/weightingFunctionInterface.hpp"
#include "strus/summarizerFunctionInterface.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/constants.hpp"
#include "errorUtils.hpp"
#include "internationalization.hpp"
#include <string>
#include <cstring>
#include <memory>

using namespace strus;

StorageObjectBuilder::StorageObjectBuilder( const FileLocatorInterface* filelocator_, ErrorBufferInterface* errorhnd_)
	:m_filelocator(filelocator_)
	,m_queryProcessor( strus::createQueryProcessor(filelocator_,errorhnd_))
	,m_storage(strus::createStorageType_std(filelocator_,errorhnd_))
	,m_statsprocmap()
	,m_errorhnd(errorhnd_)
{
	if (!m_queryProcessor.get()) throw strus::runtime_error(_TXT("error creating '%s'"), "query processor");
	if (!m_storage.get()) throw strus::runtime_error(_TXT("error creating '%s'"), "storage");

	DatabaseReference dbref( strus::createDatabaseType_leveldb( m_filelocator, m_errorhnd));
	if (!dbref.get()) throw strus::runtime_error( _TXT( "failed to create handle for default key value store database '%s'"), "leveldb");
	m_dbmap[ strus::Constants::leveldb_database_name()] = dbref;
	m_dbmap[ ""] = dbref;

	StatisticsProcessorReference spref( strus::createStatisticsProcessor_std( m_filelocator, m_errorhnd));
	if (!spref.get()) throw std::runtime_error( _TXT( "failed to create handle for default statistics processor"));
	m_statsprocmap[ strus::Constants::standard_statistics_processor()] = spref;
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
		m_errorhnd->report( ErrorCodeOperationOrder, _TXT( "cannot add storage module with previous unhandled errors"));
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
				m_errorhnd->report( ErrorCodeRuntimeError, _TXT("error creating posting join operator"));
				return;
			}
			else
			{
				m_queryProcessor->definePostingJoinOperator( pi->name, func);
				if (m_errorhnd->hasError())
				{
					delete func;
					m_errorhnd->report( ErrorCodeRuntimeError, _TXT("error defining posting join operator"));
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
				m_errorhnd->report( ErrorCodeRuntimeError, _TXT("error creating weighting function"));
				return;
			}
			m_queryProcessor->defineWeightingFunction( wi->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report( ErrorCodeRuntimeError, _TXT("error defining weighting function"));
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
				m_errorhnd->report( ErrorCodeRuntimeError, _TXT("error creating summarizer function '%s'"), si->name);
				return;
			}
			m_queryProcessor->defineSummarizerFunction( si->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report( ErrorCodeRuntimeError, _TXT("error defining summarizer function '%s'"), si->name);
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
				m_errorhnd->report( ErrorCodeRuntimeError, _TXT("error creating scalar function parser '%s'"), si->name);
				return;
			}
			m_queryProcessor->defineScalarFunctionParser( si->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report( ErrorCodeRuntimeError, _TXT("error defining scalar function parser '%s'"), si->name);
				return;
			}
		}
	}
	try
	{
		m_storageModules.push_back( mod);

		if (mod->databaseConstructor.create && mod->databaseConstructor.name)
		{
			DatabaseReference dbref( mod->databaseConstructor.create( m_filelocator, m_errorhnd));
			if (!dbref.get()) throw strus::runtime_error( _TXT( "failed to create data base Constructor loaded from module: '%s': %s"), mod->databaseConstructor.name, m_errorhnd->fetchError());
			m_dbmap[ string_conv::tolower( mod->databaseConstructor.name)] = dbref;
		}
		if (mod->statisticsProcessorConstructor.create && mod->statisticsProcessorConstructor.name)
		{
			StatisticsProcessorReference spref( mod->statisticsProcessorConstructor.create( m_errorhnd));
			if (!spref.get()) throw strus::runtime_error( _TXT( "failed to create statistics processor Constructor loaded from module: '%s': %s"), mod->statisticsProcessorConstructor.name, m_errorhnd->fetchError());
			m_statsprocmap[ string_conv::tolower( mod->statisticsProcessorConstructor.name)] = spref;
		}
		if (mod->vectorStorageConstructor.create && mod->vectorStorageConstructor.name)
		{
			VectorStorageReference ref( mod->vectorStorageConstructor.create( m_filelocator, m_errorhnd));
			if (!ref.get()) throw strus::runtime_error( _TXT( "failed to create vector storage interface loaded from module: '%s': %s"), mod->vectorStorageConstructor.name, m_errorhnd->fetchError());
			m_vsmodelmap[ string_conv::tolower( mod->vectorStorageConstructor.name)] = ref;
		}
	}
	CATCH_ERROR_MAP( _TXT("failed to add storage module: %s"), *m_errorhnd);
}

const DatabaseInterface* StorageObjectBuilder::getDatabase( const std::string& name) const
{
	try
	{
		std::map<std::string,DatabaseReference>::const_iterator
			di = m_dbmap.find( string_conv::tolower( name));
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
			si = m_statsprocmap.find( string_conv::tolower( name));
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

const VectorStorageInterface* StorageObjectBuilder::getVectorStorage( const std::string& name) const
{
	try
	{
		std::map<std::string,VectorStorageReference>::const_iterator
			si = m_vsmodelmap.find( string_conv::tolower( name));
		if (si == m_vsmodelmap.end())
		{
			throw strus::runtime_error( _TXT( "undefined vector storage interface '%s'"), name.c_str());
		}
		else
		{
			return si->second.get();
		}
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error getting vector storage interface from storage object builder: %s"), *m_errorhnd, 0);
}

const StorageInterface* StorageObjectBuilder::getStorage() const
{
	return m_storage.get();
}

QueryEvalInterface* StorageObjectBuilder::createQueryEval() const
{
	return strus::createQueryEval( m_errorhnd);
}

SentenceAnalyzerInstanceInterface* StorageObjectBuilder::createSentenceAnalyzer( const std::string& name) const
{
	try
	{
		if (name.empty() || string_conv::tolower( name) == strus::Constants::standard_sentence_analyzer())
		{
			return createSentenceAnalyzerInstance_std( m_errorhnd);
		}
		else
		{
			throw strus::runtime_error(_TXT("unknown sentence analyzer type: '%s'"), name.c_str());
		}
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error creating sentence analyzer: %s"), *m_errorhnd, 0);
}


