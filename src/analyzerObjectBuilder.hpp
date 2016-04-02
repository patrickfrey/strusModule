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
#include "strus/reference.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/textProcessorInterface.hpp"
#include <string>
#include <vector>

namespace strus
{
/// \brief Forward declaration
struct AnalyzerModule;
/// \brief Forward declaration
class DocumentAnalyzerInterface;
/// \brief Forward declaration
class QueryAnalyzerInterface;
/// \brief Forward declaration
class TextProcessorInterface;
/// \brief Forward declaration
class SegmenterInterface;
/// \brief Forward declaration
class AnalyzerErrorBufferInterface;

/// \brief Implementation of AnalyzerObjectBuilderInterface for the module loader
class AnalyzerObjectBuilder
	:public AnalyzerObjectBuilderInterface
{
public:
	explicit AnalyzerObjectBuilder( AnalyzerErrorBufferInterface* errorhnd_);
	virtual ~AnalyzerObjectBuilder(){}

	virtual const TextProcessorInterface* getTextProcessor() const;
	virtual DocumentAnalyzerInterface* createDocumentAnalyzer( const std::string& segmenterName) const;
	virtual QueryAnalyzerInterface* createQueryAnalyzer() const;
	virtual SegmenterInterface* createSegmenter( const std::string& segmenterName) const;

public/*ModuleLoader*/:
	void addAnalyzerModule( const AnalyzerModule* mod);
	void addResourcePath( const std::string& path);

private:
	std::vector<const AnalyzerModule*> m_analyzerModules;
	Reference<TextProcessorInterface> m_textProcessor;
	AnalyzerErrorBufferInterface* m_errorhnd;			///< buffer for reporting errors
};

}//namespace
#endif

