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
#include "strus/moduleAnalyzer.hpp"
#include <string>

using namespace strus;

AnalyzerModule::AnalyzerModule(
		const SegmenterConstructor* segmenterConstructors_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer)
{
	init( 0,0,0,segmenterConstructors_,tokenizerConstructors_,normalizerConstructors_);
	//... no need to make query/document analyzer and textprocessor loadable by module yet
}

void AnalyzerModule::init(
		const QueryAnalyzerConstructor* queryAnalyzerConstructor_,
		const DocumentAnalyzerConstructor* documentAnalyzerConstructor_,
		const TextProcessorConstructor* textProcessorConstructor_,
		const SegmenterConstructor* segmenterConstructors_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_)
{
	queryAnalyzerConstructor.function = (queryAnalyzerConstructor_)?queryAnalyzerConstructor_->function:0;
	documentAnalyzerConstructor.function = (documentAnalyzerConstructor_)?documentAnalyzerConstructor_->function:0;
	textProcessorConstructor.function = (textProcessorConstructor_)?textProcessorConstructor_->function:0;
	segmenterConstructors = segmenterConstructors_;
	tokenizerConstructors = tokenizerConstructors_;
	normalizerConstructors = normalizerConstructors_;
}

