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
#ifndef _STRUS_MODULE_ANALYZER_HPP_INCLUDED
#define _STRUS_MODULE_ANALYZER_HPP_INCLUDED
#include "strus/moduleEntryPoint.hpp"
#include <string>

namespace strus
{

/// \brief Forward declaration
class QueryAnalyzerInterface;
/// \brief Forward declaration
class DocumentAnalyzerInterface;
/// \brief Forward declaration
class TextProcessorInterface;
/// \brief Forward declaration
class SegmenterInterface;
/// \brief Forward declaration
class TokenizerInterface;
/// \brief Forward declaration
class NormalizerInterface;

struct QueryAnalyzerConstructor
{
	typedef QueryAnalyzerInterface* (*Function)(
			const TextProcessorInterface* textProcessor);
	Function function;
};

struct DocumentAnalyzerConstructor
{
	typedef DocumentAnalyzerInterface* (*Function)( 
			const TextProcessorInterface* textProcessor,
			SegmenterInterface* segmenter);

	Function function;
};

struct TextProcessorConstructor
{
	typedef TextProcessorInterface* (*Function)();
	Function function;
};

struct SegmenterConstructor
{
	typedef SegmenterInterface* (*Function)();
	const char* name;
	Function function;
};

struct TokenizerConstructor
{
	typedef TokenizerInterface* (*Function)();
	const char* name;
	Function function;
};

struct NormalizerConstructor
{
	typedef NormalizerInterface* (*Function)();
	const char* name;
	Function function;
};


struct AnalyzerModule
	:public ModuleEntryPoint
{
	/// \brief Analyzer module constructor
	/// \param[in] segmenterConstructor_ (0,0) terminated list of segmenters or 0
	/// \param[in] tokenizerConstructor_ (0,0) terminated list of tokenizers or 0
	/// \param[in] normalizerConstructor_ (0,0) terminated list of normalizers or 0
	AnalyzerModule(
		const SegmenterConstructor* segmenterConstructors_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_);

	QueryAnalyzerConstructor queryAnalyzerConstructor;		///< query analyzer constructor
	DocumentAnalyzerConstructor documentAnalyzerConstructor;	///< document analyzer constructor
	TextProcessorConstructor textProcessorConstructor;		///< text processor constructor
	const SegmenterConstructor* segmenterConstructors;		///< 0 terminated list of segmenters
	const TokenizerConstructor* tokenizerConstructors;		///< 0 terminated list of tokenizers
	const NormalizerConstructor* normalizerConstructors;		///< 0 terminated list of normalizers

private:
	void init(
		const QueryAnalyzerConstructor* queryAnalyzerConstructor_,
		const DocumentAnalyzerConstructor* documentAnalyzerConstructor_,
		const TextProcessorConstructor* textProcessorConstructor_,
		const SegmenterConstructor* segmenterConstructors_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_);
};
}//namespace
#endif

