/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "analyzerObjectBuilder.hpp"
#include "strus/lib/textproc.hpp"
#include "strus/lib/analyzer.hpp"
#include "strus/lib/detector_std.hpp"
#include "strus/lib/contentstats_std.hpp"
#include "strus/lib/filelocator.hpp"
#include "strus/fileLocatorInterface.hpp"
#include "strus/analyzerModule.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/configParser.hpp"
#include "strus/tokenizerFunctionInterface.hpp"
#include "strus/normalizerFunctionInterface.hpp"
#include "strus/aggregatorFunctionInterface.hpp"
#include "strus/patternLexerInterface.hpp"
#include "strus/patternMatcherInterface.hpp"
#include "strus/segmenterInterface.hpp"
#include "strus/contentStatisticsInterface.hpp"
#include "strus/posTaggerInterface.hpp"
#include "strus/posTaggerInstanceInterface.hpp"
#include "internationalization.hpp"
#include "errorUtils.hpp"
#include <string>
#include <cstring>

using namespace strus;
using namespace strus::module;

AnalyzerObjectBuilder::AnalyzerObjectBuilder( const FileLocatorInterface* filelocator_, ErrorBufferInterface* errorhnd_)
	:m_textproc( strus::createTextProcessor(filelocator_,errorhnd_)),m_errorhnd(errorhnd_),m_filelocator(filelocator_)
{
	if (!m_textproc.get()) throw std::runtime_error( _TXT("error creating text processor"));
	m_docdetect.reset( strus::createDetector_std( m_textproc.get(), m_errorhnd));
	if (!m_docdetect.get()) throw std::runtime_error( _TXT("error creating document class detector"));
}

const TextProcessorInterface* AnalyzerObjectBuilder::getTextProcessor() const
{
	return m_textproc.get();
}

void AnalyzerObjectBuilder::addAnalyzerModule( const AnalyzerModule* mod)
{
	try
	{
		if (m_errorhnd->hasError())
		{
			m_errorhnd->explain(_TXT("cannot add analyzer module with previous unhandled errors: %s"));
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
					m_errorhnd->explain(_TXT("error creating tokenizer function: %s"));
					return;
				}
				m_textproc->defineTokenizer( ti->name, func);
				if (m_errorhnd->hasError())
				{
					delete func;
					m_errorhnd->explain(_TXT("error defining tokenizer function '%s'"));
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
					m_errorhnd->explain(_TXT("error creating normalizer function: %s"));
					return;
				}
				m_textproc->defineNormalizer( ni->name, func);
				if (m_errorhnd->hasError())
				{
					delete func;
					m_errorhnd->explain(_TXT("error defining normalizer function: %s"));
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
					m_errorhnd->explain(_TXT("error creating aggregator function: %s"));
					return;
				}
				m_textproc->defineAggregator( ni->name, func);
				if (m_errorhnd->hasError())
				{
					delete func;
					m_errorhnd->explain(_TXT("error defining aggregator function: %s"));
				}
			}
		}
		if (mod->segmenterConstructor.name && mod->segmenterConstructor.create)
		{
			Reference<SegmenterInterface> segref( mod->segmenterConstructor.create( m_errorhnd));
			if (segref.get())
			{
				m_textproc->defineSegmenter( mod->segmenterConstructor.name, segref.release());
			}
		}
		if (mod->patternLexerConstructor.name && mod->patternLexerConstructor.create)
		{
			PatternLexerInterface* func = mod->patternLexerConstructor.create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->explain(_TXT("error creating pattern lexer: %s"));
				return;
			}
			m_textproc->definePatternLexer( mod->patternLexerConstructor.name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->explain(_TXT("error defining pattern lexer: %s"));
			}
		}
		if (mod->patternMatcherConstructor.name && mod->patternMatcherConstructor.create)
		{
			PatternMatcherInterface* func = mod->patternMatcherConstructor.create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->explain(_TXT("error creating pattern lexer: %s"));
				return;
			}
			m_textproc->definePatternMatcher( mod->patternLexerConstructor.name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->explain(_TXT("error defining pattern lexer: %s"));
			}
		}
		m_analyzerModules.push_back( mod);
	}
	CATCH_ERROR_MAP( _TXT("error adding analyzer module: %s"), *m_errorhnd);
}

DocumentAnalyzerInstanceInterface* AnalyzerObjectBuilder::createDocumentAnalyzer(
		const SegmenterInterface* segmenter,
		const analyzer::SegmenterOptions& opts) const
{
	return strus::createDocumentAnalyzer( m_textproc.get(), segmenter, opts, m_errorhnd);
}

PosTaggerInstanceInterface* AnalyzerObjectBuilder::createPosTaggerInstance(
		const SegmenterInterface* segmenter,
		const analyzer::SegmenterOptions& opts) const
{
	const PosTaggerInterface* postagger = m_textproc->getPosTagger();
	if (!postagger) return NULL;
	return postagger->createInstance( segmenter, opts);
}

QueryAnalyzerInstanceInterface* AnalyzerObjectBuilder::createQueryAnalyzer() const
{
	return strus::createQueryAnalyzer( m_errorhnd);
}

DocumentAnalyzerMapInterface* AnalyzerObjectBuilder::createDocumentAnalyzerMap() const
{
	return strus::createDocumentAnalyzerMap( this, m_errorhnd);
}

DocumentClassDetectorInterface* AnalyzerObjectBuilder::createDocumentClassDetector() const
{
	return strus::createDetector_std( m_textproc.get(), m_errorhnd);
}

ContentStatisticsInterface* AnalyzerObjectBuilder::createContentStatistics() const
{
	return strus::createContentStatistics_std( m_textproc.get(), m_docdetect.get(), m_errorhnd);
}

