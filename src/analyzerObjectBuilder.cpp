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
#include "internationalization.hpp"
#include "errorUtils.hpp"
#include "utils.hpp"
#include <string>
#include <cstring>

using namespace strus;

AnalyzerObjectBuilder::AnalyzerObjectBuilder( ErrorBufferInterface* errorhnd_)
	:m_textProcessor( strus::createTextProcessor(errorhnd_)),m_errorhnd(errorhnd_)
{
	if (!m_textProcessor.get()) throw strus::runtime_error( "%s", _TXT("error creating text processor"));
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
	try
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
					m_errorhnd->report(_TXT("error defining tokenizer function '%s'"), ti->name);
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
					m_errorhnd->report(_TXT("error defining normalizer function '%s'"), ni->name);
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
					m_errorhnd->report(_TXT("error defining aggregator function '%s'"), ni->name);
				}
			}
		}
		if (mod->segmenterConstructor.name && mod->segmenterConstructor.create)
		{
			Reference<SegmenterInterface> segref( mod->segmenterConstructor.create( m_errorhnd));
			if (segref.get())
			{
				m_textProcessor->defineSegmenter( mod->segmenterConstructor.name, segref.release());
			}
		}
		if (mod->patternLexerConstructor.name && mod->patternLexerConstructor.create)
		{
			PatternLexerInterface* func = mod->patternLexerConstructor.create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->report(_TXT("error creating pattern lexer '%s'"), mod->patternLexerConstructor.name);
				return;
			}
			m_textProcessor->definePatternLexer( mod->patternLexerConstructor.name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report(_TXT("error defining pattern lexer '%s'"), mod->patternLexerConstructor.name);
			}
		}
		if (mod->patternMatcherConstructor.name && mod->patternMatcherConstructor.create)
		{
			PatternMatcherInterface* func = mod->patternMatcherConstructor.create( m_errorhnd);
			if (!func)
			{
				m_errorhnd->report(_TXT("error creating pattern lexer '%s'"), mod->patternMatcherConstructor.name);
				return;
			}
			m_textProcessor->definePatternMatcher( mod->patternLexerConstructor.name, func);
			if (m_errorhnd->hasError())
			{
				delete func;
				m_errorhnd->report(_TXT("error defining pattern lexer '%s'"), mod->patternMatcherConstructor.name);
			}
		}
		m_analyzerModules.push_back( mod);
	}
	CATCH_ERROR_MAP( _TXT("error adding analyzer module: %s"), *m_errorhnd);
}

DocumentAnalyzerInterface* AnalyzerObjectBuilder::createDocumentAnalyzer(
		const SegmenterInterface* segmenter,
		const analyzer::SegmenterOptions& opts) const
{
	return strus::createDocumentAnalyzer( m_textProcessor.get(), segmenter, opts, m_errorhnd);
}

QueryAnalyzerInterface* AnalyzerObjectBuilder::createQueryAnalyzer() const
{
	return strus::createQueryAnalyzer( m_errorhnd);
}


