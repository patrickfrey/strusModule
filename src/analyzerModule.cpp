/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/analyzerModule.hpp"
#include "strus/private/dll_tags.hpp"
#include <string>

using namespace strus;

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const DocumentClassDetectorConstructor& documentClassDetectorConstructor_,
		const SegmenterConstructor& segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer)
{
	init( &documentClassDetectorConstructor_, &segmenterConstructor_, tokenizerConstructors_, normalizerConstructors_, aggregatorConstructors_);
	//... no need to make query/document analyzer and textprocessor loadable by module yet
}

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const SegmenterConstructor& segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer)
{
	init( 0, &segmenterConstructor_, tokenizerConstructors_, normalizerConstructors_, aggregatorConstructors_);
	//... no need to make query/document analyzer and textprocessor loadable by module yet
}

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer)
{
	init( 0, 0, tokenizerConstructors_, normalizerConstructors_, aggregatorConstructors_);
	//... no need to make query/document analyzer and textprocessor loadable by module yet
}

void AnalyzerModule::init(
		const DocumentClassDetectorConstructor* documentClassDetectorConstructor_,
		const SegmenterConstructor* segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_)
{
	if (documentClassDetectorConstructor_)
	{
		documentClassDetectorConstructor.title = documentClassDetectorConstructor_->title;
		documentClassDetectorConstructor.create = documentClassDetectorConstructor_->create;
	}
	else
	{
		documentClassDetectorConstructor.title = 0;
		documentClassDetectorConstructor.create = 0;
	}
	if (segmenterConstructor_)
	{
		segmenterConstructor.name = segmenterConstructor_->name;
		segmenterConstructor.create = segmenterConstructor_->create;
	}
	else
	{
		segmenterConstructor.name = 0;
		segmenterConstructor.create = 0;
	}
	tokenizerConstructors = tokenizerConstructors_;
	normalizerConstructors = normalizerConstructors_;
	aggregatorConstructors = aggregatorConstructors_;
}

