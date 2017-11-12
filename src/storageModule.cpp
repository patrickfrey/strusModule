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
		const DatabaseConstructor* databaseConstructor_,
		const char* version_3rdparty_, const char* license_3rdparty_)
	:ModuleEntryPoint(ModuleEntryPoint::Storage, STRUS_STORAGE_VERSION_MAJOR, STRUS_STORAGE_VERSION_MINOR, version_3rdparty_, license_3rdparty_)
{
	init( databaseConstructor_, 0, 0, 0, 0, 0, 0);
}

DLL_PUBLIC StorageModule::StorageModule(
		const VectorStorageConstructor* vectorStorageConstructor_,
		const char* version_3rdparty_, const char* license_3rdparty_)
	:ModuleEntryPoint(ModuleEntryPoint::Storage, STRUS_STORAGE_VERSION_MAJOR, STRUS_STORAGE_VERSION_MINOR, version_3rdparty_, license_3rdparty_)
{
	init( 0, 0, vectorStorageConstructor_, 0, 0, 0, 0);
}

DLL_PUBLIC StorageModule::StorageModule(
		const PostingIteratorJoinConstructor* postingIteratorJoinConstructor_,
		const WeightingFunctionConstructor* weightingFunctionConstructor_,
		const SummarizerFunctionConstructor* summarizerFunctionConstructor_,
		const char* version_3rdparty_, const char* license_3rdparty_)
	:ModuleEntryPoint(ModuleEntryPoint::Storage, STRUS_STORAGE_VERSION_MAJOR, STRUS_STORAGE_VERSION_MINOR, version_3rdparty_, license_3rdparty_)
{
	init( 0, 0, 0, postingIteratorJoinConstructor_, weightingFunctionConstructor_, summarizerFunctionConstructor_, 0);
	//... no need to make database loadable by module yet
}

void StorageModule::init(
		const DatabaseConstructor* databaseConstructor_,
		const StatisticsProcessorConstructor* statisticsProcessorConstructor_,
		const VectorStorageConstructor* vectorStorageConstructor_,
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
	if (vectorStorageConstructor_)
	{
		vectorStorageConstructor.name = vectorStorageConstructor_->name;
		vectorStorageConstructor.create = vectorStorageConstructor_->create;
	}
	else
	{
		vectorStorageConstructor.name = 0;
		vectorStorageConstructor.create = 0;
	}
	postingIteratorJoinConstructor = postingIteratorJoinConstructor_;
	weightingFunctionConstructor = weightingFunctionConstructor_;
	summarizerFunctionConstructor = summarizerFunctionConstructor_;
	scalarFunctionParserConstructor = scalarFunctionParserConstructor_;
}



