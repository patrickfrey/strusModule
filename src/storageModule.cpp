/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/storageModule.hpp"
#include "strus/versionStorage.hpp"
#include "strus/base/dll_tags.hpp"
#include <cstring>

using namespace strus;

DLL_PUBLIC StorageModule::StorageModule(
		const DatabaseConstructor* databaseConstructor_)
	:ModuleEntryPoint(ModuleEntryPoint::Storage, STRUS_STORAGE_VERSION_MAJOR, STRUS_STORAGE_VERSION_MINOR)
{
	init( databaseConstructor_, 0, 0, 0, 0, 0, 0);
}

DLL_PUBLIC StorageModule::StorageModule(
		const VectorSpaceModelConstructor* vectorSpaceModelConstructor_)
	:ModuleEntryPoint(ModuleEntryPoint::Storage, STRUS_STORAGE_VERSION_MAJOR, STRUS_STORAGE_VERSION_MINOR)
{
	init( 0, 0, vectorSpaceModelConstructor_, 0, 0, 0, 0);
}

DLL_PUBLIC StorageModule::StorageModule(
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_)
	:ModuleEntryPoint(ModuleEntryPoint::Storage, STRUS_STORAGE_VERSION_MAJOR, STRUS_STORAGE_VERSION_MINOR)
{
	init( 0, 0, 0, postingIteratorJoinConstructor_, weightingFunctionConstructor_, summarizerFunctionConstructor_, 0);
	//... no need to make database loadable by module yet
}

void StorageModule::init(
		const DatabaseConstructor* databaseConstructor_,
		const StatisticsProcessorConstructor* statisticsProcessorConstructor_,
		const VectorSpaceModelConstructor* vectorSpaceModelConstructor_,
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_,
		const ScalarFunctionParserConstructor* scalarFunctionParserConstructor_)
{
	if (databaseConstructor_)
	{
		databaseConstructor.name = databaseConstructor_->name;
		databaseConstructor.create = databaseConstructor_->create;
	}
	else
	{
		databaseConstructor.name = 0;
		databaseConstructor.create = 0;
	}
	if (statisticsProcessorConstructor_)
	{
		statisticsProcessorConstructor.name = statisticsProcessorConstructor_->name;
		statisticsProcessorConstructor.create = statisticsProcessorConstructor_->create;
	}
	else
	{
		statisticsProcessorConstructor.name = 0;
		statisticsProcessorConstructor.create = 0;
	}
	if (vectorSpaceModelConstructor_)
	{
		vectorSpaceModelConstructor.name = vectorSpaceModelConstructor_->name;
		vectorSpaceModelConstructor.create = vectorSpaceModelConstructor_->create;
	}
	else
	{
		vectorSpaceModelConstructor.name = 0;
		vectorSpaceModelConstructor.create = 0;
	}
	postingIteratorJoinConstructor = postingIteratorJoinConstructor_;
	weightingFunctionConstructor = weightingFunctionConstructor_;
	summarizerFunctionConstructor = summarizerFunctionConstructor_;
	scalarFunctionParserConstructor = scalarFunctionParserConstructor_;
}



