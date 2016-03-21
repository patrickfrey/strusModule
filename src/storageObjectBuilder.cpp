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
#include "strus/private/fileio.hpp"
#include "strus/private/configParser.hpp"
#include "utils.hpp"
#include "errorUtils.hpp"
#include "internationalization.hpp"
#include <string>
#include <cstring>
#include <memory>

using namespace strus;

StorageObjectBuilder::StorageObjectBuilder( const char* statsprocname_, ErrorBufferInterface* errorhnd_)
	:m_queryProcessor( strus::createQueryProcessor(errorhnd_)),m_storage(strus::createStorage(errorhnd_)),m_statsprocname(statsprocname_),m_errorhnd(errorhnd_)
{
	if (!m_queryProcessor.get()) throw strus::runtime_error(_TXT("error creating '%s'"), "query processor");
	if (!m_storage.get()) throw strus::runtime_error(_TXT("error creating '%s'"), "storage");
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
				m_errorhnd->report(_TXT("error creating summarizer function"));
				return;
			}
			m_queryProcessor->defineSummarizerFunction( si->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report(_TXT("error defining summarizer function"));
				return;
			}
		}
	}
	try
	{
		m_storageModules.push_back( mod);
	}
	catch (const std::bad_alloc&)
	{
		m_errorhnd->report(_TXT("out of memory constructing storage object builder"));
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
		std::string::iterator ni = name.begin(), ne = name.end();
		for (; ni != ne; ++ ni) *ni = ::tolower(*ni);
		std::map<std::string,DatabaseReference>::const_iterator di = m_dbmap.find( name);
		if (di == m_dbmap.end())
		{
			std::vector<const StorageModule*>::const_iterator
				mi = m_storageModules.begin(), 
				me = m_storageModules.end();
			for (; mi != me; ++mi)
			{
				if ((*mi)->databaseReference.create)
				{
					if (name.empty() || utils::caseInsensitiveEquals( name, (*mi)->databaseReference.name))
					{
						DatabaseReference dbref( (*mi)->databaseReference.create( m_errorhnd));
						if (!dbref.get()) return 0;
						m_dbmap[ name] = dbref;
						return dbref.get();
					}
				}
			}
			if (name.empty() || utils::caseInsensitiveEquals( name, "leveldb"))
			{
				DatabaseReference dbref( strus::createDatabase_leveldb( m_errorhnd));
				if (!dbref.get()) return 0;
				m_dbmap[ name] = dbref;
				return dbref.get();
			}
			throw strus::runtime_error( _TXT( "undefined key value store database '%s'"), name.c_str());
		}
		else
		{
			return di->second.get();
		}
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error getting database from storage object builder: %s"), *m_errorhnd, 0);
}

const StatisticsProcessorInterface* StorageObjectBuilder::getStatisticsProcessor() const
{
	try
	{
		if (!m_statsproc.get())
		{
			std::vector<const StorageModule*>::const_iterator
				mi = m_storageModules.begin(), 
				me = m_storageModules.end();
			for (; mi != me; ++mi)
			{
				if ((*mi)->statisticsProcessorReference.create)
				{
					if (!m_statsprocname[0] || utils::caseInsensitiveEquals( m_statsprocname, (*mi)->statisticsProcessorReference.name))
					{
						m_statsproc.reset( (*mi)->statisticsProcessorReference.create( m_errorhnd));
						break;
					}
				}
			}
			if (mi == me)
			{
				if (!m_statsprocname[0] || utils::caseInsensitiveEquals( m_statsprocname, "standard"))
				{
					m_statsproc.reset( strus::createStatisticsProcessor( m_errorhnd));
				}
				else
				{
					throw strus::runtime_error( _TXT( "undefined statistics message processor '%s'"), m_statsprocname);
				}
			}
		}
		return m_statsproc.get();
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error getting statistics message processor from storage object builder: %s"), *m_errorhnd, 0);
}

const StorageInterface* StorageObjectBuilder::getStorage() const
{
	return m_storage.get();
}

StorageClientInterface* StorageObjectBuilder::createStorageClient( const std::string& config) const
{
	try
	{
		std::string dbname;
		std::string configstr( config);
	
		const DatabaseInterface* dbi = getDatabase( configstr);
		if (!dbi)
		{
			m_errorhnd->explain(_TXT("could not get database: %s"));
			return 0;
		}
		(void)strus::extractStringFromConfigString( dbname, configstr, "database", m_errorhnd);

		const StorageInterface* sti = getStorage();
		if (!sti)
		{
			m_errorhnd->explain(_TXT("could not get storage: %s"));
			return 0;
		}
		std::string databasecfg( configstr);
		std::string storagecfg( configstr);
		strus::removeKeysFromConfigString(
				databasecfg,
				sti->getConfigParameters( strus::StorageInterface::CmdCreateClient), m_errorhnd);
	
		strus::removeKeysFromConfigString(
				storagecfg,
				dbi->getConfigParameters( strus::DatabaseInterface::CmdCreateClient), m_errorhnd);
		//... In storagecfg is now the pure storage configuration without the database settings
		if (m_errorhnd->hasError())
		{
			m_errorhnd->explain(_TXT("cannot create database client: %s"));
			return 0;
		}
		std::auto_ptr<DatabaseClientInterface> database( dbi->createClient( databasecfg));
		if (!database.get())
		{
			m_errorhnd->report(_TXT("error creating database client"));
			return 0;
		}
		const StatisticsProcessorInterface* statsproc = 0;
		if (m_statsprocname)
		{
			statsproc = getStatisticsProcessor();
			if (!statsproc)
			{
				m_errorhnd->report(_TXT("error creating statistics message processor"));
				return 0;
			}
		}
		std::auto_ptr<StorageClientInterface>
			storage( sti->createClient( storagecfg, database.get(), statsproc));
		if (!storage.get())
		{
			m_errorhnd->report(_TXT("error creating storage client"));
			return 0;
		}
		(void)database.release(); //... ownership passed to storage
		return storage.release(); //... ownership returned
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error storage object builder creating storage: %s"), *m_errorhnd, 0);
}


StorageAlterMetaDataTableInterface* StorageObjectBuilder::createAlterMetaDataTable( const std::string& config) const
{
	try
	{
		std::string dbname;
		std::string configstr( config);

		const DatabaseInterface* dbi = getDatabase( configstr);
		const StorageInterface* sti = getStorage();

		(void)strus::extractStringFromConfigString( dbname, configstr, "database", m_errorhnd);
		std::string databasecfg( configstr);
		strus::removeKeysFromConfigString(
				databasecfg,
				sti->getConfigParameters( strus::StorageInterface::CmdCreateClient), m_errorhnd);
		//... In storagecfg is now the pure storage configuration without the database settings
		if (m_errorhnd->hasError())
		{
			m_errorhnd->explain(_TXT("cannot evaluate database: %s"));
			return 0;
		}
		std::auto_ptr<DatabaseClientInterface> database( dbi->createClient( databasecfg));
		if (!database.get())
		{
			m_errorhnd->report(_TXT("error creating database client"));
			return 0;
		}
		std::auto_ptr<StorageAlterMetaDataTableInterface> altermetatable( sti->createAlterMetaDataTable( database.get()));
		if (!altermetatable.get())
		{
			m_errorhnd->report(_TXT("error creating alter metadata table client"));
			return 0;
		}
		(void)database.release(); //... ownership passed to alter metadata table client
		return altermetatable.release(); //... ownership returned
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error storage object builder creating alter meta data table: %s"), *m_errorhnd, 0);
}


QueryEvalInterface* StorageObjectBuilder::createQueryEval() const
{
	return strus::createQueryEval( m_errorhnd);
}


