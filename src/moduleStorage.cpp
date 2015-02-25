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
#include "strus/moduleStorage.hpp"
#include <cstring>

using namespace strus;

void DatabaseConstructor::init()
{
	std::memset( this, 0, sizeof(*this));
}

void DatabaseConstructor::init( const DatabaseConstructor& o)
{
	std::memcpy( this, &o, sizeof(*this));
}

void StorageConstructor::init()
{
	std::memset( this, 0, sizeof(*this));
}

void StorageConstructor::init( const StorageConstructor& o)
{
	std::memcpy( this, &o, sizeof(*this));
}


StorageModule::StorageModule(
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_)
	:ModuleEntryPoint(ModuleEntryPoint::Storage)
{
	init( 0,0,0,0,postingIteratorJoinConstructor_,weightingFunctionConstructor_,summarizerFunctionConstructor_);
	//... no need to make database, storage, queryprocessor and query evaluation loadable by module yet
}

void StorageModule::init(
		const DatabaseConstructor* databaseConstructor_,
		const StorageConstructor* storageConstructor_,
		const QueryProcessorConstructor* queryProcessorConstructor_,
		const QueryEvalConstructor* queryEvalConstructor_,
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_)
{
	if (databaseConstructor_) {
		databaseConstructor.init(*databaseConstructor_);
	} else {
		databaseConstructor.init();
	}
	if (storageConstructor_) {
		storageConstructor.init(*storageConstructor_);
	} else {
		storageConstructor.init();
	}
	queryProcessorConstructor.function = (queryProcessorConstructor_)?queryProcessorConstructor_->function:0;
	queryEvalConstructor.function = (queryEvalConstructor_)?queryEvalConstructor_->function:0;
	postingIteratorJoinConstructor = postingIteratorJoinConstructor_;
	weightingFunctionConstructor = weightingFunctionConstructor_;
	summarizerFunctionConstructor = summarizerFunctionConstructor_;
}



