/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/lib/pattern_test.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/base/dll_tags.hpp"
#include "strus/analyzerModule.hpp"
#include "internationalization.hpp"
#include "errorUtils.hpp"

static const strus::PatternLexerConstructor lexer =
{
	"std", strus::createPatternLexer_test
};

static const strus::PatternMatcherConstructor matcher =
{
	"std", strus::createPatternMatcher_test
};

extern "C" DLL_PUBLIC strus::AnalyzerModule entryPoint;

strus::AnalyzerModule entryPoint( lexer, matcher);




