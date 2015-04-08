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
class SegmenterInterface;
/// \brief Forward declaration
class TokenizerConstructorInterface;
/// \brief Forward declaration
class NormalizerConstructorInterface;

/// \brief Structure to define a segmenter for an alternative document format (like XML) as module
struct SegmenterConstructor
{
	typedef SegmenterInterface* (*Create)();
	const char* name;			///< name of the segmenter
	Create create;				///< segmenter constructor
};

/// \brief Structure to define a proprietary tokenizer of text segments as module
struct TokenizerConstructor
{
	typedef const TokenizerConstructorInterface* (*Function)();
	const char* name;			///< name of the tokenizer
	Function function;			///< tokenizer function
};

/// \brief Structure to define a proprietary normalizer of tokens as module
struct NormalizerConstructor
{
	typedef const NormalizerConstructorInterface* (*Function)();
	const char* name;			///< name of the normalizer
	Function function;			///< normalizer function
};


struct AnalyzerModule
	:public ModuleEntryPoint
{
	/// \brief Analyzer module constructor
	/// \param[in] segmenterConstructor_ segmenter definition
	/// \param[in] tokenizerConstructor_ (0,0) terminated list of tokenizers or 0
	/// \param[in] normalizerConstructor_ (0,0) terminated list of normalizers or 0
	AnalyzerModule(
		const SegmenterConstructor& segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_);

	/// \brief Analyzer module constructor without segmenter definition
	/// \param[in] tokenizerConstructor_ (0,0) terminated list of tokenizers or 0
	/// \param[in] normalizerConstructor_ (0,0) terminated list of normalizers or 0
	AnalyzerModule(
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_);

	SegmenterConstructor segmenterConstructor;			///< a segmenter definition
	const TokenizerConstructor* tokenizerConstructors;		///< 0 terminated list of tokenizers
	const NormalizerConstructor* normalizerConstructors;		///< 0 terminated list of normalizers

private:
	void init(
		const SegmenterConstructor* segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_);
};
}//namespace
#endif

