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
#include "strus/storageModule.hpp"
#include "strus/private/dll_tags.hpp"
#include <cstring>

using namespace strus;

DLL_PUBLIC StorageModule::StorageModule(
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_)
	:ModuleEntryPoint(ModuleEntryPoint::Storage)
{
	init( 0, 0, postingIteratorJoinConstructor_, weightingFunctionConstructor_, summarizerFunctionConstructor_);
	//... no need to make database loadable by module yet
}

void StorageModule::init(
		const DatabaseReference* databaseReference_,
		const StatisticsProcessorReference* statisticsProcessorReference_,
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_)
{
	if (databaseReference_)
	{
		databaseReference.name = databaseReference_->name;
		databaseReference.create = databaseReference_->create;
	}
	else
	{
		databaseReference.name = 0;
		databaseReference.create = 0;
	}
	if (statisticsProcessorReference_)
	{
		statisticsProcessorReference.name = statisticsProcessorReference_->name;
		statisticsProcessorReference.create = statisticsProcessorReference_->create;
	}
	else
	{
		statisticsProcessorReference.name = 0;
		statisticsProcessorReference.create = 0;
	}
	postingIteratorJoinConstructor = postingIteratorJoinConstructor_;
	weightingFunctionConstructor = weightingFunctionConstructor_;
	summarizerFunctionConstructor = summarizerFunctionConstructor_;
}



