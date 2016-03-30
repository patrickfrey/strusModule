/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
	const char* m_statsprocname;				///< statistics message processor selected
	ErrorBufferInterface* m_errorhnd;			///< buffer for reporting errors
};

}//namespace
#endif

