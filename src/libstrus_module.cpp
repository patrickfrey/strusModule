/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/lib/module.hpp"
#include "moduleLoader.hpp"
#include "private/dll_tags.hpp"
#include "strus/errorBufferInterface.hpp"
#include "internationalization.hpp"
#include "errorUtils.hpp"

using namespace strus;

DLL_PUBLIC ModuleLoaderInterface* strus::createModuleLoader( ErrorBufferInterface* errorhnd)
{
	try
	{
		static bool intl_initialized = false;
		if (!intl_initialized)
		{
			strus::initMessageTextDomain();
			intl_initialized = true;
		}
		return new ModuleLoader( errorhnd);
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error creating module loader"), *errorhnd, 0);
}

