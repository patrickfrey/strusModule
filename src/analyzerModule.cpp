/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/analyzerModule.hpp"
#include "strus/versionAnalyzer.hpp"
#include "strus/base/dll_tags.hpp"
#include <string>

using namespace strus;

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const DocumentClassDetectorConstructor& documentClassDetectorConstructor_,
		const SegmenterConstructor& segmenterConstructor_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer, STRUS_ANALYZER_VERSION_MAJOR, STRUS_ANALYZER_VERSION_MINOR)
{
	init( &documentClassDetectorConstructor_, &segmenterConstructor_, 0, 0, 0, 0, 0);
}

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const SegmenterConstructor& segmenterConstructor_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer, STRUS_ANALYZER_VERSION_MAJOR, STRUS_ANALYZER_VERSION_MINOR)
{
	init( 0, &segmenterConstructor_, 0, 0, 0, 0, 0);
}

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer, STRUS_ANALYZER_VERSION_MAJOR, STRUS_ANALYZER_VERSION_MINOR)
{
	init( 0, 0, tokenizerConstructors_, normalizerConstructors_, aggregatorConstructors_, 0, 0);
}

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const PatternLexerConstructor& patternLexerConstructor_,
		const PatternMatcherConstructor& patternMatcherConstructor_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer, STRUS_ANALYZER_VERSION_MAJOR, STRUS_ANALYZER_VERSION_MINOR)
{
	init( 0, 0, 0, 0, 0, &patternLexerConstructor_, &patternMatcherConstructor_);
}

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const PatternLexerConstructor& patternLexerConstructor_,
		const PatternMatcherConstructor& patternMatcherConstructor_,
		const char* version_3rdparty, const char* license_3rdparty)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer, STRUS_ANALYZER_VERSION_MAJOR, STRUS_ANALYZER_VERSION_MINOR, version_3rdparty, license_3rdparty)
{
	init( 0, 0, 0, 0, 0, &patternLexerConstructor_, &patternMatcherConstructor_);
}

void AnalyzerModule::init(
		const DocumentClassDetectorConstructor* documentClassDetectorConstructor_,
		const SegmenterConstructor* segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_,
		const PatternLexerConstructor* patternLexerConstructor_,
		const PatternMatcherConstructor* patternMatcherConstructor_)
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
	if (patternLexerConstructor_)
	{
		patternLexerConstructor.name = patternLexerConstructor_->name;
		patternLexerConstructor.create = patternLexerConstructor_->create;
	}
	else
	{
		patternLexerConstructor.name = 0;
		patternLexerConstructor.create = 0;
	}
	if (patternMatcherConstructor_)
	{
		patternMatcherConstructor.name = patternMatcherConstructor_->name;
		patternMatcherConstructor.create = patternMatcherConstructor_->create;
	}
	else
	{
		patternMatcherConstructor.name = 0;
		patternMatcherConstructor.create = 0;
	}
}

