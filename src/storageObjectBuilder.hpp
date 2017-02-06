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
#include "strus/storageInterface.hpp"
#include "strus/databaseInterface.hpp"
#include "strus/segmenterInterface.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "strus/vectorStorageInterface.hpp"
#include <string>
#include <vector>
#include <map>

namespace strus
{
/// \brief Forward declaration
struct StorageModule;
/// \brief Forward declaration
class QueryEvalInterface;
/// \brief Forward declaration
class ErrorBufferInterface;

/// \brief Implementation of StorageObjectBuilderInterface for the module loader
class StorageObjectBuilder
	:public StorageObjectBuilderInterface
{
public:
	explicit StorageObjectBuilder( ErrorBufferInterface* errorhnd_);
	virtual ~StorageObjectBuilder(){}

	virtual const StorageInterface* getStorage() const;
	virtual const DatabaseInterface* getDatabase( const std::string& config) const;
	virtual const QueryProcessorInterface* getQueryProcessor() const;
	virtual const StatisticsProcessorInterface* getStatisticsProcessor( const std::string& name) const;
	virtual const VectorStorageInterface* getVectorStorage( const std::string& name) const;
	virtual QueryEvalInterface* createQueryEval() const;

public/*ModuleLoader*/:
	void addStorageModule( const StorageModule* mod);

private:
	std::vector<const StorageModule*> m_storageModules;			///< loaded modules
	Reference<QueryProcessorInterface> m_queryProcessor;			///< query processor handle
	Reference<StorageInterface> m_storage;					///< storage handle
	typedef Reference<DatabaseInterface> DatabaseReference;
	std::map<std::string,DatabaseReference> m_dbmap;			///< cached database handles
	typedef Reference<StatisticsProcessorInterface> StatisticsProcessorReference;
	std::map<std::string,StatisticsProcessorReference> m_statsprocmap;	///< statistics processor interface map
	typedef Reference<VectorStorageInterface> VectorStorageReference;
	std::map<std::string,VectorStorageReference> m_vsmodelmap;		///< vector space model handle map
	ErrorBufferInterface* m_errorhnd;					///< buffer for reporting errors
};

}//namespace
#endif

