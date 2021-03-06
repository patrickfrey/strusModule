/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Storage module object type declarations
/// \file storageModule.hpp
#ifndef _STRUS_MODULE_STORAGE_HPP_INCLUDED
#define _STRUS_MODULE_STORAGE_HPP_INCLUDED
#include "strus/moduleEntryPoint.hpp"
#include <string>
#include <cstring>

/// \brief strus toplevel namespace
namespace strus
{

/// \brief Forward declaration
class DatabaseInterface;
/// \brief Forward declaration
class StatisticsProcessorInterface;
/// \brief Forward declaration
class VectorStorageInterface;
/// \brief Forward declaration
class WeightingFunctionInterface;
/// \brief Forward declaration
class SummarizerFunctionInterface;
/// \brief Forward declaration
class PostingJoinOperatorInterface;
/// \brief Forward declaration
class ScalarFunctionParserInterface;
/// \brief Forward declaration
class FileLocatorInterface;
/// \brief Forward declaration
class ErrorBufferInterface;


/// \brief Structure to declare the key value store database to use by the storage as module object
struct DatabaseConstructor
{
	typedef DatabaseInterface* (*Create)( const FileLocatorInterface* filelocator, ErrorBufferInterface* errorhnd);
	const char* name;				///< name of the database implementation
	Create create;					///< constructor of the object
};

/// \brief Structure to declare an alternative packing/unpacking of statistics messages
struct StatisticsProcessorConstructor
{
	typedef StatisticsProcessorInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;				///< name of the implementation
	Create create;					///< constructor of the object
};

/// \brief Structure to declare an alternative vector storage to map vectors to sets of features
struct VectorStorageConstructor
{
	typedef VectorStorageInterface* (*Create)( const FileLocatorInterface* filelocator, ErrorBufferInterface* errorhnd);
	const char* name;				///< name of the implementation
	Create create;					///< constructor of the object
};

/// \brief Structure to declare an operator to join sets of postings represented as iterator as module object
struct PostingIteratorJoinConstructor
{
	typedef PostingJoinOperatorInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;				///< name of the join operator
	Create create;					///< constructor of the function
};

/// \brief Structure to declare a query evaluation weighting function as module object
struct WeightingFunctionConstructor
{
	typedef WeightingFunctionInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;				///< name of the weighting function
	Create create;					///< constructor of the function
};

/// \brief Structure to declare a query evaluation summarizer function as module object
struct SummarizerFunctionConstructor
{
	typedef SummarizerFunctionInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;				///< name of the summarizer function
	Create create;					///< constructor of the function
};

/// \brief Structure to declare a parser, resp. compiler for scalar function definition source
struct ScalarFunctionParserConstructor
{
	typedef ScalarFunctionParserInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;				///< name of the parser implementation
	Create create;					///< constructor of the object
};


/// \brief Structure that contains all storage module objects
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
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_,
		const char* version_3rdparty=0, const char* license_3rdparty=0);

	/// \brief Storage module constructor for alternative database implementation
	/// \param[in] databaseConstructor_ alternative database implementation
	explicit StorageModule(
		const DatabaseConstructor* databaseConstructor_,
		const char* version_3rdparty=0, const char* license_3rdparty=0);

	/// \brief Storage module constructor for alternative vector storage interface
	/// \param[in] vectorStorageConstructor_ alternative vector storage interface
	explicit StorageModule(
		const VectorStorageConstructor* vectorStorageConstructor_,
		const char* version_3rdparty=0, const char* license_3rdparty=0);

	DatabaseConstructor databaseConstructor;				///< alternative key value store database 
	StatisticsProcessorConstructor statisticsProcessorConstructor;		///< alternative packing/unpacking of statistics messages
	VectorStorageConstructor vectorStorageConstructor;			///< alternative vectorspace model for mapping vectors to features
	const PostingIteratorJoinConstructor* postingIteratorJoinConstructor;	///< join operator function for postings
	const WeightingFunctionConstructor* weightingFunctionConstructor;	///< alternative weighting functions for ranking
	const SummarizerFunctionConstructor* summarizerFunctionConstructor;	///< summarizer functions
	const ScalarFunctionParserConstructor* scalarFunctionParserConstructor;	///< scalar function parsers

private:
	void init(
		const DatabaseConstructor* databaseConstructor_,
		const StatisticsProcessorConstructor* statisticsProcessorConstructor_,
		const VectorStorageConstructor* vectorStorageConstructor_,
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_,
		const ScalarFunctionParserConstructor* scalarFunctionParserConstructor_);
};
}//namespace
#endif

