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
#include "strus/analyzerModule.hpp"
#include "strus/private/dll_tags.hpp"
#include <string>

using namespace strus;

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const SegmenterConstructor& segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer)
{
	init( &segmenterConstructor_, tokenizerConstructors_, normalizerConstructors_);
	//... no need to make query/document analyzer and textprocessor loadable by module yet
}

DLL_PUBLIC AnalyzerModule::AnalyzerModule(
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_)
	:ModuleEntryPoint(ModuleEntryPoint::Analyzer)
{
	init( 0, tokenizerConstructors_, normalizerConstructors_);
	//... no need to make query/document analyzer and textprocessor loadable by module yet
}

void AnalyzerModule::init(
		const SegmenterConstructor* segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_)
{
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
}

