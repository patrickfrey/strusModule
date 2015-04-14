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
class AnalyzerModule;
/// \brief Forward declaration
class DocumentAnalyzerInterface;
/// \brief Forward declaration
class QueryAnalyzerInterface;
/// \brief Forward declaration
class TextProcessorInterface;
/// \brief Forward declaration
class SegmenterInterface;


/// \brief Implementation of AnalyzerObjectBuilderInterface for the module loader
class AnalyzerObjectBuilder
	:public AnalyzerObjectBuilderInterface
{
public:
	AnalyzerObjectBuilder();
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
};

}//namespace
#endif

