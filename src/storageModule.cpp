/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
	init( 0, 0, postingIteratorJoinConstructor_, weightingFunctionConstructor_, summarizerFunctionConstructor_, 0);
	//... no need to make database loadable by module yet
}

void StorageModule::init(
		const DatabaseReference* databaseReference_,
		const StatisticsProcessorReference* statisticsProcessorReference_,
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_,
		const ScalarFunctionParserConstructor* scalarFunctionParserConstructor_)
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
	scalarFunctionParserConstructor = scalarFunctionParserConstructor_;
}



