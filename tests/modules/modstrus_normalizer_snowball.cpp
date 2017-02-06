/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/base/dll_tags.hpp"
#include "strus/analyzerModule.hpp"
#include "strus/lib/normalizer_snowball.hpp"
#include "strus/strus.hpp"

static const strus::NormalizerConstructor normalizers[] =
{
	{"stem", &strus::createNormalizer_snowball},
	{0,0}	
};

extern "C" DLL_PUBLIC strus::AnalyzerModule entryPoint;

strus::AnalyzerModule entryPoint( 0, normalizers, 0);




