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
#ifndef _STRUS_MODULE_STORAGE_OBJECT_BUILDER_HPP_INCLUDED
#define _STRUS_MODULE_STORAGE_OBJECT_BUILDER_HPP_INCLUDED
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/reference.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/textProcessorInterface.hpp"
#include <string>
#include <vector>
#include <map>

namespace strus
{
/// \brief Forward declaration
class StorageModule;
/// \brief Forward declaration
class StorageClientInterface;
/// \brief Forward declaration
class StorageAlterMetaDataTableInterface;
/// \brief Forward declaration
class StorageInterface;
/// \brief Forward declaration
class QueryEvalInterface;
/// \brief Forward declaration
class QueryProcessorInterface;
/// \brief Forward declaration
class QueryAnalyzerInterface;
/// \brief Forward declaration
class DatabaseInterface;
/// \brief Forward declaration
class StatisticsProcessorInterface;
/// \brief Forward declaration
class ErrorBufferInterface;

/// \brief Implementation of StorageObjectBuilderInterface for the module loader
class StorageObjectBuilder
	:public StorageObjectBuilderInterface
{
public:
	StorageObjectBuilder( const char* statsprocname_, ErrorBufferInterface* errorhnd_);
	virtual ~StorageObjectBuilder(){}

	virtual const StorageInterface* getStorage() const;
	virtual const DatabaseInterface* getDatabase( const std::string& config) const;
	virtual const QueryProcessorInterface* getQueryProcessor() const;
	virtual const StatisticsProcessorInterface* getStatisticsProcessor() const;

	virtual StorageClientInterface* createStorageClient( const std::string& config) const;
	virtual QueryEvalInterface* createQueryEval() const;
	virtual StorageAlterMetaDataTableInterface* createAlterMetaDataTable( const std::string& config) const;

public/*ModuleLoader*/:
	void addStorageModule( const StorageModule* mod);

private:
	std::vector<const StorageModule*> m_storageModules;	///< loaded modules
	Reference<QueryProcessorInterface> m_queryProcessor;	///< query processor handle
	Reference<StorageInterface> m_storage;			///< storage handle
	typedef Reference<DatabaseInterface> DatabaseReference;
	mutable std::map<std::string,DatabaseReference> m_dbmap;///< cached database handles
	mutable Reference<StatisticsProcessorInterface> m_statsproc;
	const char* m_statsprocname;				///< peer message processor selected
	ErrorBufferInterface* m_errorhnd;			///< buffer for reporting errors
};

}//namespace
#endif

