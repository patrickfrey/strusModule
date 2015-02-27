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
#include "strus/lib/textprocessor.hpp"
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
#include <string>
#include <cstring>
#include <memory>
#include <boost/algorithm/string.hpp>

using namespace strus;

ObjectBuilder::ObjectBuilder()
	:m_queryProcessor( strus::createQueryProcessor())
	,m_textProcessor( strus::createTextProcessor())
{}

void ObjectBuilder::addAnalyzerModule( const AnalyzerModule* mod)
{
	TokenizerConstructor const* ti = mod->tokenizerConstructors;
	for (; ti->function != 0; ++ti)
	{
		m_textProcessor->defineTokenizer( ti->name, ti->function());
	}
	NormalizerConstructor const* ni = mod->normalizerConstructors;
	for (; ni->function != 0; ++ni)
	{
		m_textProcessor->defineNormalizer( ni->name, ni->function());
	}
}

void ObjectBuilder::addStorageModule( const StorageModule* mod)
{
	PostingIteratorJoinConstructor const* pi = mod->postingIteratorJoinConstructor;
	for (; pi->function != 0; ++pi)
	{
		m_queryProcessor->definePostingJoinOperator( pi->name, pi->function());
	}
	WeightingFunctionConstructor const* wi = mod->weightingFunctionConstructor;
	for (; wi->function != 0; ++wi)
	{
		m_queryProcessor->defineWeightingFunction( wi->name, wi->function());
	}
	SummarizerFunctionConstructor const* si = mod->summarizerFunctionConstructor;
	for (; si->function != 0; ++si)
	{
		m_queryProcessor->defineSummarizerFunction( si->name, si->function());
	}
}

const DatabaseInterface* ObjectBuilder::getDatabase( const std::string& name) const
{
	std::vector<const StorageModule*>::const_iterator
		mi = m_storageModules.begin(), 
		me = m_storageModules.end();
	for (; mi != me; ++mi)
	{
		if (name.empty() || boost::algorithm::iequals( name, (*mi)->databaseReference.name))
		{
			return (*mi)->databaseReference.get();
		}
	}
	if (name.empty() || boost::algorithm::iequals( name, "leveldb"))
	{
		return strus::getDatabase_leveldb();
	}
	throw std::runtime_error( std::string( "undefined key value store database '") + name + "'");
}

SegmenterInterface* ObjectBuilder::createSegmenter( const std::string& name) const
{
	std::vector<const AnalyzerModule*>::const_iterator
		ai = m_analyzerModules.begin(), 
		ae = m_analyzerModules.end();
	for (; ai != ae; ++ai)
	{
		if (name.empty() || boost::algorithm::iequals( name, (*ai)->segmenterConstructor.name))
		{
			return (*ai)->segmenterConstructor.create();
		}
	}
	if (name.empty() || boost::algorithm::iequals( name, "textwolf"))
	{
		return createSegmenter_textwolf();
	}
	throw std::runtime_error( std::string( "undefined segmenter '") + name + "'");
}

StorageClientInterface* ObjectBuilder::createStorageClient( const std::string& config) const
{
	std::string dbname;
	std::string configstr( config);

	(void)strus::extractStringFromConfigString( dbname, configstr, "database");

	const DatabaseInterface* dbi = getDatabase( dbname);
	const StorageInterface* sti = strus::getStorage();

	std::string databasecfg( configstr);
	std::string storagecfg( configstr);
	strus::removeKeysFromConfigString(
			databasecfg,
			sti->getConfigParameters( strus::StorageInterface::CmdCreateClient));

	strus::removeKeysFromConfigString(
			storagecfg,
			dbi->getConfigParameters( strus::DatabaseInterface::CmdCreateClient));
	//... In storage_cfg is now the pure storage configuration without the database settings

	std::auto_ptr<DatabaseClientInterface> database( dbi->createClient( databasecfg));
	std::auto_ptr<StorageClientInterface> storage( sti->createClient( storagecfg, database.get()));
	(void)database.release(); //... ownership passed to storage
	return storage.release(); //... ownership returned
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


