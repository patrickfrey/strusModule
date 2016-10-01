/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Main header file with all include of (strus,strusAnalyzer,strusModule)
/// \file strus.hpp
#ifndef _STRUS_MAIN_INCLUDE_HPP_INCLUDED
#define _STRUS_MAIN_INCLUDE_HPP_INCLUDED
// Basic storage data types:
#include "strus/index.hpp"
#include "strus/constants.hpp"
#include "strus/numericVariant.hpp"
#include "strus/reference.hpp"
#include "strus/errorBufferInterface.hpp"

// Component versions
#include "strus/versionBase.hpp"
#include "strus/versionStorage.hpp"
#include "strus/versionAnalyzer.hpp"
#include "strus/versionModule.hpp"

// Key/value store database used by the storage to store its persistent data:
#include "strus/lib/database_leveldb.hpp"
#include "strus/databaseOptions.hpp"
#include "strus/databaseInterface.hpp"
#include "strus/databaseClientInterface.hpp"
#include "strus/databaseCursorInterface.hpp"
#include "strus/databaseBackupCursorInterface.hpp"
#include "strus/databaseTransactionInterface.hpp"

// Storage (storage structure that defines all data tables needed for query evaluation):
#include "strus/storageObjectBuilderInterface.hpp"
#include "strus/lib/storage.hpp"
#include "strus/storageInterface.hpp"
#include "strus/storageClientInterface.hpp"
#include "strus/storageTransactionInterface.hpp"
#include "strus/storageDocumentInterface.hpp"
#include "strus/documentTermIteratorInterface.hpp"
#include "strus/valueIteratorInterface.hpp"
#include "strus/invAclIteratorInterface.hpp"
#include "strus/metaDataReaderInterface.hpp"
#include "strus/metaDataRestrictionInstanceInterface.hpp"
#include "strus/metaDataRestrictionInterface.hpp"
#include "strus/storageAlterMetaDataTableInterface.hpp"
#include "strus/attributeReaderInterface.hpp"
#include "strus/forwardIteratorInterface.hpp"
#include "strus/postingIteratorInterface.hpp"
#include "strus/postingJoinOperatorInterface.hpp"
#include "strus/lib/statsproc.hpp"
#include "strus/statisticsProcessorInterface.hpp"
#include "strus/statisticsViewerInterface.hpp"
#include "strus/statisticsIteratorInterface.hpp"
#include "strus/statisticsBuilderInterface.hpp"
#include "strus/storageDocumentUpdateInterface.hpp"
#include "strus/storageDumpInterface.hpp"

// Query processor (functions for the query evaluation used for ranking and summarization):
#include "strus/lib/queryproc.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/summarizerFunctionContextInterface.hpp"
#include "strus/summarizerFunctionInterface.hpp"
#include "strus/summarizerFunctionInstanceInterface.hpp"
#include "strus/weightingFunctionContextInterface.hpp"
#include "strus/weightingFunctionInterface.hpp"
#include "strus/weightingFunctionInstanceInterface.hpp"
#include "strus/lib/scalarfunc.hpp"
#include "strus/scalarFunctionInstanceInterface.hpp"
#include "strus/scalarFunctionInterface.hpp"
#include "strus/scalarFunctionParserInterface.hpp"

// Query evaluation (processing a query to get a ranked list of documents with attributes):
#include "strus/lib/queryeval.hpp"
#include "strus/queryEvalInterface.hpp"
#include "strus/queryInterface.hpp"
#include "strus/weightedDocument.hpp"
#include "strus/resultDocument.hpp"

// Document/query analyzer (create from text a structure suitable to insert into a storage):
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/lib/analyzer.hpp"
#include "strus/queryAnalyzerInterface.hpp"
#include "strus/documentAnalyzerInterface.hpp"
#include "strus/documentAnalyzerContextInterface.hpp"
#include "strus/analyzer/attribute.hpp"
#include "strus/analyzer/document.hpp"
#include "strus/analyzer/metaData.hpp"
#include "strus/analyzer/term.hpp"

// Text processor (functions for the document analysis to produce index terms, attributes and meta data out of segments of text):
#include "strus/lib/textproc.hpp"
#include "strus/textProcessorInterface.hpp"

// Document detection
#include "strus/lib/detector_std.hpp"
#include "strus/analyzer/documentClass.hpp"
#include "strus/documentClassDetectorInterface.hpp"

// Document segmenter (segmenting a document into typed text segments that can be processed by the analyzer):
#include "strus/lib/segmenter_textwolf.hpp"
#include "strus/segmenterContextInterface.hpp"
#include "strus/segmenterInstanceInterface.hpp"
#include "strus/segmenterInterface.hpp"

// Tokenizer (functions for the text processor to split a text segment into tokens):
#include "strus/lib/tokenizer_word.hpp"
#include "strus/lib/tokenizer_punctuation.hpp"
#include "strus/analyzer/token.hpp"
#include "strus/tokenizerFunctionContextInterface.hpp"
#include "strus/tokenizerFunctionInterface.hpp"
#include "strus/tokenizerFunctionInstanceInterface.hpp"

// Token normalizer (functions for the text processor to map tokens to normalized terms for the storage):
#include "strus/lib/normalizer_charconv.hpp"
#include "strus/lib/normalizer_dateconv.hpp"
#include "strus/lib/normalizer_dictmap.hpp"
#include "strus/lib/normalizer_snowball.hpp"
#include "strus/normalizerFunctionContextInterface.hpp"
#include "strus/normalizerFunctionInterface.hpp"
#include "strus/normalizerFunctionInstanceInterface.hpp"

// Aggregators (functions for values aggregated from many document properties like statistics)
#include "strus/lib/aggregator_vsm.hpp"
#include "strus/aggregatorFunctionInterface.hpp"
#include "strus/aggregatorFunctionInstanceInterface.hpp"

// Loading strus objects from modules
#include "strus/analyzerModule.hpp"
#include "strus/storageModule.hpp"
#include "strus/moduleEntryPoint.hpp"
#include "strus/moduleLoaderInterface.hpp"

// Method Call Trace
#include "strus/traceElement.hpp"
#include "strus/traceLoggerInterface.hpp"
#include "strus/traceObjectBuilderInterface.hpp"

// Version info
#include "strus/versionBase.hpp"
#include "strus/versionAnalyzer.hpp"
#include "strus/versionStorage.hpp"
#include "strus/versionTrace.hpp"
#include "strus/versionModule.hpp"

#endif

