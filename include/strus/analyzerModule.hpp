/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Analyzer module object type declarations
/// \file analyzerModule.hpp
#ifndef _STRUS_MODULE_ANALYZER_HPP_INCLUDED
#define _STRUS_MODULE_ANALYZER_HPP_INCLUDED
#include "strus/moduleEntryPoint.hpp"
#include <string>

/// \brief strus toplevel namespace
namespace strus
{

/// \brief Forward declaration
class DocumentClassDetectorInterface;
/// \brief Forward declaration
class SegmenterInterface;
/// \brief Forward declaration
class TokenizerFunctionInterface;
/// \brief Forward declaration
class NormalizerFunctionInterface;
/// \brief Forward declaration
class AggregatorFunctionInterface;
/// \brief Forward declaration
class ErrorBufferInterface;

/// \brief Structure to define a content detector for an alternative document format
struct DocumentClassDetectorConstructor
{
	typedef const DocumentClassDetectorInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* title;			///< title of the detector (informal name)
	Create create;				///< constructor
};

/// \brief Structure to define a segmenter for an alternative document format (like XML) as module
struct SegmenterConstructor
{
	typedef SegmenterInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;			///< name of the segmenter
	Create create;				///< constructor
};

/// \brief Structure to define a proprietary tokenizer of text segments as module
struct TokenizerConstructor
{
	typedef TokenizerFunctionInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;			///< name of the tokenizer
	Create create;				///< constructor
};

/// \brief Structure to define a proprietary normalizer of tokens as module
struct NormalizerConstructor
{
	typedef NormalizerFunctionInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;			///< name of the normalizer
	Create create;				///< constructor
};

/// \brief Structure to define a proprietary function to aggregate a value for a document as meta data (counting, classification, etc.)
struct AggregatorConstructor
{
	typedef AggregatorFunctionInterface* (*Create)( ErrorBufferInterface* errorhnd);
	const char* name;			///< name of the aggregator
	Create create;				///< constructor
};


/// \brief Structure that contains all analyzer module objects
struct AnalyzerModule
	:public ModuleEntryPoint
{
	/// \brief Analyzer module constructor
	/// \param[in] DocumentClassDetectorConstructor_ content detector definition
	/// \param[in] segmenterConstructor_ segmenter definition
	/// \param[in] tokenizerConstructors_ (0,0) terminated list of tokenizers or 0
	/// \param[in] normalizerConstructors_ (0,0) terminated list of normalizers or 0
	/// \param[in] aggregatorConstructors_ (0,0) terminated list of aggregators or 0
	AnalyzerModule(
		const DocumentClassDetectorConstructor& DocumentClassDetectorConstructor_,
		const SegmenterConstructor& segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_);

	/// \brief Analyzer module constructor
	/// \param[in] DocumentClassDetectorConstructor_ content detector definition
	/// \param[in] segmenterConstructor_ segmenter definition
	/// \param[in] tokenizerConstructors_ (0,0) terminated list of tokenizers or 0
	/// \param[in] normalizerConstructors_ (0,0) terminated list of normalizers or 0
	/// \param[in] aggregatorConstructors_ (0,0) terminated list of aggregators or 0
	AnalyzerModule(
		const SegmenterConstructor& segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_);

	/// \brief Analyzer module constructor without segmenter definition
	/// \param[in] tokenizerConstructors_ (0,0) terminated list of tokenizers or 0
	/// \param[in] normalizerConstructors_ (0,0) terminated list of normalizers or 0
	/// \param[in] aggregatorConstructors_ (0,0) terminated list of statistic functions or 0
	AnalyzerModule(
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_);

	DocumentClassDetectorConstructor documentClassDetectorConstructor;	///< a content detector definition
	SegmenterConstructor segmenterConstructor;				///< a segmenter definition
	const TokenizerConstructor* tokenizerConstructors;			///< 0 terminated list of tokenizers
	const NormalizerConstructor* normalizerConstructors;			///< 0 terminated list of normalizers
	const AggregatorConstructor* aggregatorConstructors;			///< 0 terminated list of aggregators

private:
	void init(
		const DocumentClassDetectorConstructor* documentClassDetectorConstructor_,
		const SegmenterConstructor* segmenterConstructor_,
		const TokenizerConstructor* tokenizerConstructors_,
		const NormalizerConstructor* normalizerConstructors_,
		const AggregatorConstructor* aggregatorConstructors_);
};
}//namespace
#endif

