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
#include "strus/analyzerErrorBufferInterface.hpp"
#include "strus/private/fileio.hpp"
#include "strus/private/configParser.hpp"
#include "strus/tokenizerFunctionInterface.hpp"
#include "strus/normalizerFunctionInterface.hpp"
#include "strus/aggregatorFunctionInterface.hpp"
#include "internationalization.hpp"
#include "errorUtils.hpp"
#include "utils.hpp"
#include <string>
#include <cstring>
#include <memory>

using namespace strus;

AnalyzerObjectBuilder::AnalyzerObjectBuilder( AnalyzerErrorBufferInterface* errorhnd_)
	:m_textProcessor( strus::createTextProcessor(errorhnd_)),m_errorhnd(errorhnd_)
{
	if (!m_textProcessor.get()) throw strus::runtime_error(_TXT("error creating text processor"));
}

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
	if (!m_errorhnd->hasError())
	{
		m_errorhnd->report(_TXT("cannot add analyzer module with previous unhandled errors"));
		return;
	}
	if (mod->tokenizerConstructors)
	{
		TokenizerConstructor const* ti = mod->tokenizerConstructors;
		for (; ti->create != 0; ++ti)
		{
			TokenizerFunctionInterface* func = ti->create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->report(_TXT("error creating tokenizer function"));
				return;
			}
			m_textProcessor->defineTokenizer( ti->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report(_TXT("error defining tokenizer function"));
			}
		}
	}
	if (mod->normalizerConstructors)
	{
		NormalizerConstructor const* ni = mod->normalizerConstructors;
		for (; ni->create != 0; ++ni)
		{
			NormalizerFunctionInterface* func = ni->create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->report(_TXT("error creating normalizer function"));
				return;
			}
			m_textProcessor->defineNormalizer( ni->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report(_TXT("error defining normalizer function"));
			}
		}
	}
	if (mod->aggregatorConstructors)
	{
		AggregatorConstructor const* ni = mod->aggregatorConstructors;
		for (; ni->create != 0; ++ni)
		{
			AggregatorFunctionInterface* func = ni->create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->report(_TXT("error creating aggregator function"));
				return;
			}
			m_textProcessor->defineAggregator( ni->name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report("error defining aggregator function");
			}
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
			return (*ai)->segmenterConstructor.create( m_errorhnd);
		}
	}
	if (segmenterName.empty() || utils::caseInsensitiveEquals( segmenterName, "textwolf"))
	{
		return createSegmenter_textwolf( m_errorhnd);
	}
	m_errorhnd->report(_TXT( "undefined segmenter '%s'"), segmenterName.c_str());
	return 0;
}


DocumentAnalyzerInterface* AnalyzerObjectBuilder::createDocumentAnalyzer( const std::string& segmenterName) const
{
	std::auto_ptr<SegmenterInterface> segmenter( createSegmenter( segmenterName));
	if (!segmenter.get())
	{
		m_errorhnd->report(_TXT("error creating segmenter"));
		return 0;
	}
	DocumentAnalyzerInterface* rt = strus::createDocumentAnalyzer( segmenter.get(), m_errorhnd);
	return rt;
}


QueryAnalyzerInterface* AnalyzerObjectBuilder::createQueryAnalyzer() const
{
	return strus::createQueryAnalyzer( m_errorhnd);
}


