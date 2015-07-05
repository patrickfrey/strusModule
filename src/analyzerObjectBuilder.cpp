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
#include "analyzerObjectBuilder.hpp"
#include "strus/lib/textproc.hpp"
#include "strus/lib/segmenter_textwolf.hpp"
#include "strus/lib/analyzer.hpp"
#include "strus/analyzerModule.hpp"
#include "strus/private/fileio.hpp"
#include "strus/private/configParser.hpp"
#include "utils.hpp"
#include <string>
#include <cstring>
#include <memory>

using namespace strus;

AnalyzerObjectBuilder::AnalyzerObjectBuilder()
	:m_textProcessor( strus::createTextProcessor())
{}

const TextProcessorInterface* AnalyzerObjectBuilder::getTextProcessor() const
{
	return m_textProcessor.get();
}

void AnalyzerObjectBuilder::addResourcePath( const std::string& path)
{
	m_textProcessor->addResourcePath( path);
}

void AnalyzerObjectBuilder::addAnalyzerModule( const AnalyzerModule* mod)
{
	if (mod->tokenizerConstructors)
	{
		TokenizerConstructor const* ti = mod->tokenizerConstructors;
		for (; ti->function != 0; ++ti)
		{
			m_textProcessor->defineTokenizer( ti->name, ti->function());
		}
	}
	if (mod->normalizerConstructors)
	{
		NormalizerConstructor const* ni = mod->normalizerConstructors;
		for (; ni->function != 0; ++ni)
		{
			m_textProcessor->defineNormalizer( ni->name, ni->function());
		}
	}
	if (mod->aggregatorConstructors)
	{
		AggregatorConstructor const* ni = mod->aggregatorConstructors;
		for (; ni->function != 0; ++ni)
		{
			m_textProcessor->defineAggregator( ni->name, ni->function());
		}
	}
	m_analyzerModules.push_back( mod);
}

SegmenterInterface* AnalyzerObjectBuilder::createSegmenter( const std::string& segmenterName) const
{
	std::vector<const AnalyzerModule*>::const_iterator
		ai = m_analyzerModules.begin(), 
		ae = m_analyzerModules.end();
	for (; ai != ae; ++ai)
	{
		if ((*ai)->segmenterConstructor.name
		&&  (segmenterName.empty() || utils::caseInsensitiveEquals( segmenterName, (*ai)->segmenterConstructor.name)))
		{
			return (*ai)->segmenterConstructor.create();
		}
	}
	if (segmenterName.empty() || utils::caseInsensitiveEquals( segmenterName, "textwolf"))
	{
		return createSegmenter_textwolf();
	}
	throw std::runtime_error( std::string( "undefined segmenter '") + segmenterName + "'");
}


DocumentAnalyzerInterface* AnalyzerObjectBuilder::createDocumentAnalyzer( const std::string& segmenterName) const
{
	std::auto_ptr<SegmenterInterface> segmenter( createSegmenter( segmenterName));
	DocumentAnalyzerInterface* rt = strus::createDocumentAnalyzer( segmenter.get());
	(void)segmenter.release(); //... ownership passed to analyzer created
	return rt;
}


QueryAnalyzerInterface* AnalyzerObjectBuilder::createQueryAnalyzer() const
{
	return strus::createQueryAnalyzer();
}


