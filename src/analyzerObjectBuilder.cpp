/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "analyzerObjectBuilder.hpp"
#include "strus/lib/textproc.hpp"
#include "strus/lib/segmenter_textwolf.hpp"
#include "strus/lib/segmenter_cjson.hpp"
#include "strus/lib/segmenter_tsv.hpp"
#include "strus/lib/analyzer.hpp"
#include "strus/analyzerModule.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/configParser.hpp"
#include "strus/tokenizerFunctionInterface.hpp"
#include "strus/normalizerFunctionInterface.hpp"
#include "strus/aggregatorFunctionInterface.hpp"
#include "strus/segmenterInterface.hpp"
#include "internationalization.hpp"
#include "errorUtils.hpp"
#include "utils.hpp"
#include <string>
#include <cstring>

using namespace strus;

AnalyzerObjectBuilder::AnalyzerObjectBuilder( ErrorBufferInterface* errorhnd_)
	:m_textProcessor( strus::createTextProcessor(errorhnd_)),m_errorhnd(errorhnd_)
{
	if (!m_textProcessor.get()) throw strus::runtime_error(_TXT("error creating text processor"));
	Reference<SegmenterInterface> segref_xml( strus::createSegmenter_textwolf( m_errorhnd));
	m_segmenterMap[ "textwolf"] = segref_xml;
	m_segmenterMap[ ""] = segref_xml;
	m_mimeSegmenterMap[ utils::tolower( segref_xml->mimeType())] = segref_xml;

	Reference<SegmenterInterface> segref_cjson( strus::createSegmenter_cjson( m_errorhnd));
	m_segmenterMap[ "cjson"] = segref_cjson;
	m_mimeSegmenterMap[ utils::tolower( segref_cjson->mimeType())] = segref_cjson;

	Reference<SegmenterInterface> segref_tsv( strus::createSegmenter_tsv( m_errorhnd));
	m_segmenterMap[ "tsv"] = segref_tsv;
	m_mimeSegmenterMap[ utils::tolower( segref_tsv->mimeType())] = segref_tsv;
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
	if (m_errorhnd->hasError())
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
	if (mod->segmenterConstructor.name && mod->segmenterConstructor.create)
	{
		Reference<SegmenterInterface> segref( mod->segmenterConstructor.create( m_errorhnd));
		m_segmenterMap[ utils::tolower(mod->segmenterConstructor.name)] = segref;
		m_mimeSegmenterMap[ utils::tolower(segref->mimeType())] = segref;
	}
	m_analyzerModules.push_back( mod);
}

const SegmenterInterface* AnalyzerObjectBuilder::getSegmenter( const std::string& segmenterName) const
{
	try
	{
		SegmenterMap::const_iterator si = m_segmenterMap.find( utils::tolower( segmenterName));
		if (si == m_segmenterMap.end())
		{
			throw strus::runtime_error(_TXT("unknown document segmenter '%s'"), segmenterName.c_str());
			return 0;
		}
		return si->second.get();
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error getting segmenter by name: %s"), *m_errorhnd, 0);
}

const SegmenterInterface* AnalyzerObjectBuilder::findMimeTypeSegmenter( const std::string& mimetype) const
{
	try
	{
		SegmenterMap::const_iterator si = m_mimeSegmenterMap.find( utils::tolower( mimetype));
		if (si == m_mimeSegmenterMap.end())
		{
			throw strus::runtime_error(_TXT("no segmenter available for this MIME type: '%s'"), mimetype.c_str());
			return 0;
		}
		return si->second.get();
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error getting segmenter by MIME type: %s"), *m_errorhnd, 0);
}

DocumentAnalyzerInterface* AnalyzerObjectBuilder::createDocumentAnalyzer(
		const SegmenterInterface* segmenter,
		const analyzer::SegmenterOptions& opts) const
{
	return strus::createDocumentAnalyzer( segmenter, opts, m_errorhnd);
}

QueryAnalyzerInterface* AnalyzerObjectBuilder::createQueryAnalyzer() const
{
	return strus::createQueryAnalyzer( m_errorhnd);
}


