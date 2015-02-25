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
#ifndef _STRUS_MODULE_ANALYZER_HPP_INCLUDED
#define _STRUS_MODULE_ANALYZER_HPP_INCLUDED
#include "strus/moduleEntryPoint.hpp"
#include "strus/lib/configType.hpp"
#include <string>
#include <cstring>

namespace strus
{

/// \brief Forward declaration
class DatabaseInterface;
/// \brief Forward declaration
class StorageInterface;
/// \brief Forward declaration
class StorageAlterMetaDataTableInterface;
/// \brief Forward declaration
class QueryEvalInterface;
/// \brief Forward declaration
class QueryProcessorInterface;
/// \brief Forward declaration
class WeightingFunctionInterface;
/// \brief Forward declaration
class SummarizerFunctionInterface;
/// \brief Forward declaration
class PostingJoinOperatorInterface;


struct DatabaseConstructor
{
	typedef DatabaseInterface* (*DatabaseClientConstructor)( const std::string& configsource);
	typedef void (*CreateDatabaseFunction)( const std::string& configsource);
	typedef void (*DestroyDatabaseFunction)( const std::string& configsource);
	typedef void (*DatabaseConfigDescriptionFunction)( ConfigType type);
	typedef void (*DatabaseConfigParametersFunction)( ConfigType type);

	DatabaseClientConstructor createDatabaseClient;
	CreateDatabaseFunction createDatabase;
	DestroyDatabaseFunction destroyDatabase;
	DatabaseConfigDescriptionFunction getDatabaseConfigDescription;
	DatabaseConfigParametersFunction getDatabaseConfigParameters;

	void init();
	void init( const DatabaseConstructor& o);
};

struct StorageConstructor
{
	typedef StorageInterface* (*StorageClientConstructor)( const std::string& configsource, DatabaseInterface* database);
	typedef void (*CreateStorageFunction)( const std::string& configsource, DatabaseInterface* database);
	typedef StorageAlterMetaDataTableInterface* (*StorageAlterMetaDataTableConstructor)( DatabaseInterface* database);
	typedef void (*StorageConfigDescriptionFunction)( ConfigType type);
	typedef void (*StorageConfigParametersFunction)( ConfigType type);

	StorageClientConstructor createStorageClient;
	CreateStorageFunction createStorage;
	StorageAlterMetaDataTableConstructor storageAlterMetaDataTableConstructor;
	StorageConfigDescriptionFunction getStorageConfigDescription;
	StorageConfigParametersFunction getStorageConfigParameters;

	void init();
	void init( const StorageConstructor& o);
};

struct QueryProcessorConstructor
{
	typedef QueryProcessorInterface* (*Function)( const StorageInterface* storage);
	Function function;
};

struct QueryEvalConstructor
{
	typedef QueryEvalInterface* (*Function)( const QueryProcessorInterface* processor);
	Function function;
};

struct PostingIteratorJoinConstructor
{
	typedef PostingJoinOperatorInterface* (*Function)();
	Function function;
};

struct WeightingFunctionConstructor
{
	typedef WeightingFunctionInterface* (*Function)();
	Function function;
};

struct SummarizerFunctionConstructor
{
	typedef SummarizerFunctionInterface* (*Function)();
	Function function;
};



struct StorageModule
	:public ModuleEntryPoint
{
	/// \brief Storage module constructor
	/// \param[in] postingIteratorJoinConstructor_ (0,0) terminated list of posting iterator join operators or 0
	/// \param[in] weightingFunctionConstructor_ (0,0) terminated list of weighting functions or 0
	/// \param[in] summarizerFunctionConstructor_ (0,0) terminated list of summarizers or 0
	StorageModule(
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_);

	DatabaseConstructor databaseConstructor;
	StorageConstructor storageConstructor;
	QueryProcessorConstructor queryProcessorConstructor;
	QueryEvalConstructor queryEvalConstructor;
	const PostingIteratorJoinConstructor* postingIteratorJoinConstructor;
	const WeightingFunctionConstructor* weightingFunctionConstructor;
	const SummarizerFunctionConstructor* summarizerFunctionConstructor;

private:
	void init(
		const DatabaseConstructor* databaseConstructor_,
		const StorageConstructor* storageConstructor_,
		const QueryProcessorConstructor* queryProcessorConstructor_,
		const QueryEvalConstructor* queryEvalConstructor_,
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_);
};
}//namespace
#endif

