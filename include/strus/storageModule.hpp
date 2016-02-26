/*
---------------------------------------------------------------------
    The C++ library strus implements basic operations to build
    a search engine for structured search on unstructured data.

    Copyright (C) 2015 Patrick Frey

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------

	The latest version of strus can be found at 'http://github.com/patrickfrey/strus'
	For documentation see 'http://patrickfrey.github.com/strus'

--------------------------------------------------------------------
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
class WeightingFunctionInterface;
/// \brief Forward declaration
class SummarizerFunctionInterface;
/// \brief Forward declaration
class PostingJoinOperatorInterface;
/// \brief Forward declaration
class ErrorBufferInterface;


/// \brief Structure to declare the key value store database to use by the storage as module object
struct DatabaseReference
{
	typedef DatabaseInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;				///< name of the database implementation
	Create create;					///< constructor of the object
};

/// \brief Structure to declare an alternative packing/unpacking of statistics messages
struct StatisticsProcessorReference
{
	typedef StatisticsProcessorInterface* (*Create)( ErrorBufferInterface* errorhnd);
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
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_);

	DatabaseReference databaseReference;					///< alternative key value store database 
	StatisticsProcessorReference statisticsProcessorReference;		///< alternative packing/unpacking of statistics messages
	const PostingIteratorJoinConstructor* postingIteratorJoinConstructor;	///< join operator function for postings
	const WeightingFunctionConstructor* weightingFunctionConstructor;	///< alternative weighting functions for ranking
	const SummarizerFunctionConstructor* summarizerFunctionConstructor;	///< summarizer functions

private:
	void init(
		const DatabaseReference* databaseReference_,
		const StatisticsProcessorReference* statisticsProcessorReference_,
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_);
};
}//namespace
#endif

