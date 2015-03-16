/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2013,2014 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
*/
#include "objectBuilder.hpp"
#include "strus/lib/queryproc.hpp"
#include "strus/lib/textproc.hpp"
#include "strus/lib/queryproc.hpp"
#include "strus/lib/database_leveldb.hpp"
#include "strus/lib/segmenter_textwolf.hpp"
#include "strus/lib/storage.hpp"
#include "strus/lib/queryeval.hpp"
#include "strus/lib/analyzer.hpp"
#include "strus/storageModule.hpp"
#include "strus/analyzerModule.hpp"
#include "strus/databaseInterface.hpp"
#include "strus/databaseClientInterface.hpp"
#include "strus/storageInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/private/fileio.hpp"
#include "strus/private/configParser.hpp"
#include "utils.hpp"
#include <string>
#include <cstring>
#include <memory>

using namespace strus;

ObjectBuilder::ObjectBuilder()
	:m_queryProcessor( strus::createQueryProcessor())
	,m_textProcessor( strus::createTextProcessor())
{}

const QueryProcessorInterface* ObjectBuilder::getQueryProcessor() const
{
	return m_queryProcessor.get();
}

const TextProcessorInterface* ObjectBuilder::getTextProcessor() const
{
	return m_textProcessor.get();
}

void ObjectBuilder::addResourcePath( const std::string& path)
{
	m_textProcessor->addResourcePath( path);
}

void ObjectBuilder::addAnalyzerModule( const AnalyzerModule* mod)
{
	if (mod->tokenizerConstructors)
	{
		TokenizerConstructor const* ti = mod->tokenizerConstructors;
		for (; ti->function != 0; ++ti)
		{
			m_textProcessor->defineTokenizer( ti->name, ti->function());
		}
	}
	if (mod->normalizerConstructors)
	{
		NormalizerConstructor const* ni = mod->normalizerConstructors;
		for (; ni->function != 0; ++ni)
		{
			m_textProcessor->defineNormalizer( ni->name, ni->function());
		}
	}
	m_analyzerModules.push_back( mod);
}

void ObjectBuilder::addStorageModule( const StorageModule* mod)
{
	if (mod->postingIteratorJoinConstructor)
	{
		PostingIteratorJoinConstructor const* pi = mod->postingIteratorJoinConstructor;
		for (; pi->function != 0; ++pi)
		{
			m_queryProcessor->definePostingJoinOperator( pi->name, pi->function());
		}
	}
	if (mod->weightingFunctionConstructor)
	{
		WeightingFunctionConstructor const* wi = mod->weightingFunctionConstructor;
		for (; wi->function != 0; ++wi)
		{
			m_queryProcessor->defineWeightingFunction( wi->name, wi->function());
		}
	}
	if (mod->summarizerFunctionConstructor)
	{
		SummarizerFunctionConstructor const* si = mod->summarizerFunctionConstructor;
		for (; si->function != 0; ++si)
		{
			m_queryProcessor->defineSummarizerFunction( si->name, si->function());
		}
	}
	m_storageModules.push_back( mod);
}

const DatabaseInterface* ObjectBuilder::getDatabase( const std::string& config) const
{
	std::string configstr( config);
	std::string name;
	(void)strus::extractStringFromConfigString( name, configstr, "database");

	std::vector<const StorageModule*>::const_iterator
		mi = m_storageModules.begin(), 
		me = m_storageModules.end();
	for (; mi != me; ++mi)
	{
		if (name.empty() || utils::caseInsensitiveEquals( name, (*mi)->databaseReference.name))
		{
			return (*mi)->databaseReference.get();
		}
	}
	if (name.empty() || utils::caseInsensitiveEquals( name, "leveldb"))
	{
		return strus::getDatabase_leveldb();
	}
	throw std::runtime_error( std::string( "undefined key value store database '") + name + "'");
}

SegmenterInterface* ObjectBuilder::createSegmenter( const std::string& segmenterName) const
{
	std::vector<const AnalyzerModule*>::const_iterator
		ai = m_analyzerModules.begin(), 
		ae = m_analyzerModules.end();
	for (; ai != ae; ++ai)
	{
		if ((*ai)->segmenterConstructor.name
		&&  (segmenterName.empty() || utils::caseInsensitiveEquals( segmenterName, (*ai)->segmenterConstructor.name)))
		{
			return (*ai)->segmenterConstructor.create();
		}
	}
	if (segmenterName.empty() || utils::caseInsensitiveEquals( segmenterName, "textwolf"))
	{
		return createSegmenter_textwolf();
	}
	throw std::runtime_error( std::string( "undefined segmenter '") + segmenterName + "'");
}

const StorageInterface* ObjectBuilder::getStorage() const
{
	return strus::getStorage();
}

StorageClientInterface* ObjectBuilder::createStorageClient( const std::string& config) const
{
	std::string dbname;
	std::string configstr( config);

	(void)strus::extractStringFromConfigString( dbname, configstr, "database");

	const DatabaseInterface* dbi = getDatabase( dbname);
	const StorageInterface* sti = getStorage();

	std::string databasecfg( configstr);
	std::string storagecfg( configstr);
	strus::removeKeysFromConfigString(
			databasecfg,
			sti->getConfigParameters( strus::StorageInterface::CmdCreateClient));

	strus::removeKeysFromConfigString(
			storagecfg,
			dbi->getConfigParameters( strus::DatabaseInterface::CmdCreateClient));
	//... In storagecfg is now the pure storage configuration without the database settings

	std::auto_ptr<DatabaseClientInterface> database( dbi->createClient( databasecfg));
	std::auto_ptr<StorageClientInterface> storage( sti->createClient( storagecfg, database.get()));
	(void)database.release(); //... ownership passed to storage
	return storage.release(); //... ownership returned
}


StorageAlterMetaDataTableInterface* ObjectBuilder::createAlterMetaDataTable( const std::string& config) const
{
	std::string dbname;
	std::string configstr( config);

	(void)strus::extractStringFromConfigString( dbname, configstr, "database");

	const DatabaseInterface* dbi = getDatabase( dbname);
	const StorageInterface* sti = getStorage();

	std::string databasecfg( configstr);
	strus::removeKeysFromConfigString(
			databasecfg,
			sti->getConfigParameters( strus::StorageInterface::CmdCreateClient));
	//... In storagecfg is now the pure storage configuration without the database settings

	std::auto_ptr<DatabaseClientInterface> database( dbi->createClient( databasecfg));
	std::auto_ptr<StorageAlterMetaDataTableInterface> altermetatable( sti->createAlterMetaDataTable( database.get()));
	(void)database.release(); //... ownership passed to storage
	return altermetatable.release(); //... ownership returned
}


QueryEvalInterface* ObjectBuilder::createQueryEval() const
{
	return strus::createQueryEval( m_queryProcessor.get());
}

DocumentAnalyzerInterface* ObjectBuilder::createDocumentAnalyzer( const std::string& segmenterName) const
{
	std::auto_ptr<SegmenterInterface> segmenter( createSegmenter( segmenterName));
	DocumentAnalyzerInterface* rt = strus::createDocumentAnalyzer( m_textProcessor.get(), segmenter.get());
	(void)segmenter.release(); //... ownership passed to analyzer created
	return rt;
}

QueryAnalyzerInterface* ObjectBuilder::createQueryAnalyzer() const
{
	return strus::createQueryAnalyzer( m_textProcessor.get());
}


