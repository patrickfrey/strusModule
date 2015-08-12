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
class PeerMessageProcessorInterface;
/// \brief Forward declaration
class WeightingFunctionInterface;
/// \brief Forward declaration
class SummarizerFunctionInterface;
/// \brief Forward declaration
class PostingJoinOperatorInterface;


/// \brief Structure to declare the key value store database to use by the storage as module object
struct DatabaseReference
{
	typedef const DatabaseInterface* (*Get)();
	const char* name;				///< name of the database implementation
	Get get;					///< getter function to reference the database object
};

/// \brief Structure to declare an alternative packing/unpacking of peer messages
struct PeerMessageProcessorReference
{
	typedef const PeerMessageProcessorInterface* (*Get)();
	const char* name;				///< name of the implementation
	Get get;					///< getter function to reference the object
};

/// \brief Structure to declare an operator to join sets of postings represented as iterator as module object
struct PostingIteratorJoinConstructor
{
	typedef PostingJoinOperatorInterface* (*Function)();
	const char* name;				///< name of the join operator
	Function function;				///< join function
};

/// \brief Structure to declare a query evaluation weighting function as module object
struct WeightingFunctionConstructor
{
	typedef WeightingFunctionInterface* (*Function)();
	const char* name;				///< name of the weighting function
	Function function;				///< weighting function
};

/// \brief Structure to declare a query evaluation summarizer function as module object
struct SummarizerFunctionConstructor
{
	typedef SummarizerFunctionInterface* (*Function)();
	const char* name;				///< name of the summarizer function
	Function function;				///< summarizer function
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
	PeerMessageProcessorReference peerMessageProcessorReference;		///< alternative packing/unpacking of peer messages
	const PostingIteratorJoinConstructor* postingIteratorJoinConstructor;	///< join operator function for postings
	const WeightingFunctionConstructor* weightingFunctionConstructor;	///< alternative weighting functions for ranking
	const SummarizerFunctionConstructor* summarizerFunctionConstructor;	///< summarizer functions

private:
	void init(
		const DatabaseReference* databaseReference_,
		const PeerMessageProcessorReference* peerMessageProcessorReference_,
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_);
};
}//namespace
#endif

