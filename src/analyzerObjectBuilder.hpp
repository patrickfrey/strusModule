/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_MODULE_ANALYZER_OBJECT_BUILDER_HPP_INCLUDED
#define _STRUS_MODULE_ANALYZER_OBJECT_BUILDER_HPP_INCLUDED
#include "strus/analyzerObjectBuilderInterface.hpp"
#include "strus/documentClassDetectorInterface.hpp"
#include "strus/reference.hpp"
#include "strus/textProcessorInterface.hpp"
#include <string>
#include <vector>
#include <map>

namespace strus
{
/// \brief Forward declaration
struct AnalyzerModule;
/// \brief Forward declaration
class DocumentAnalyzerInstanceInterface;
/// \brief Forward declaration
class PosTaggerInstanceInterface;
/// \brief Forward declaration
class QueryAnalyzerInstanceInterface;
/// \brief Forward declaration
class DocumentAnalyzerMapInterface;
/// \brief Forward declaration
class ContentStatisticsInterface;
/// \brief Forward declaration
class TextProcessorInterface;
/// \brief Forward declaration
class SegmenterOptions;
/// \brief Forward declaration
class SegmenterInterface;
/// \brief Forward declaration
class ErrorBufferInterface;
/// \brief Forward declaration
class FileLocatorInterface;

/// \brief Implementation of AnalyzerObjectBuilderInterface for the module loader
class AnalyzerObjectBuilder
	:public AnalyzerObjectBuilderInterface
{
public:
	explicit AnalyzerObjectBuilder( const FileLocatorInterface* filelocator_, ErrorBufferInterface* errorhnd_);
	virtual ~AnalyzerObjectBuilder(){}

	virtual const TextProcessorInterface* getTextProcessor() const;
	virtual DocumentAnalyzerInstanceInterface* createDocumentAnalyzer(
			const SegmenterInterface* segmenter,
			const analyzer::SegmenterOptions& opts) const;
	virtual PosTaggerInstanceInterface* createPosTaggerInstance(
			const SegmenterInterface* segmenter,
			const analyzer::SegmenterOptions& opts) const;

	virtual QueryAnalyzerInstanceInterface* createQueryAnalyzer() const;
	virtual DocumentAnalyzerMapInterface* createDocumentAnalyzerMap() const;
	virtual DocumentClassDetectorInterface* createDocumentClassDetector() const;
	virtual ContentStatisticsInterface* createContentStatistics() const;

public/*ModuleLoader*/:
	void addAnalyzerModule( const AnalyzerModule* mod);

private:
	std::vector<const AnalyzerModule*> m_analyzerModules;	///< analyzer modules loader
	Reference<TextProcessorInterface> m_textproc;		///< text processor
	Reference<DocumentClassDetectorInterface> m_docdetect;	///< document class detector
	ErrorBufferInterface* m_errorhnd;			///< buffer for reporting errors
	const FileLocatorInterface* m_filelocator;		///< resources and file locator interface
};

}//namespace
#endif

