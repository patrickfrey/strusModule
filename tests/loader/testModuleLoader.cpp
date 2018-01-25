/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/lib/module.hpp"
#include "strus/lib/error.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "testModuleDirectory.hpp"
#include "strus/base/local_ptr.hpp"
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdio>

int main( int argc, const char** argv)
{
	strus::local_ptr<strus::ErrorBufferInterface> errorbuf( strus::createErrorBuffer_standard( stderr, 1));
	if (!errorbuf.get())
	{
		std::cerr << "error creating error buffer" << std::endl;
		return -1;
	}
	strus::local_ptr<strus::ModuleLoaderInterface> modloader( strus::createModuleLoader( errorbuf.get()));
	if (!modloader.get())
	{
		std::cerr << "error creating module loader" << std::endl;
		return -1;
	}
	std::cerr << "setting load module path to '" << STRUS_TEST_MODULE_DIRECTORY << "'" << std::endl;
	modloader->addModulePath( STRUS_TEST_MODULE_DIRECTORY);

	int ai = 1, ae = argc;
	for (; ai != ae; ++ai)
	{
		std::cerr << "loading module '" << argv[ai] << "'" << std::endl;
		std::vector<std::string> modfiles = modloader->moduleLoadTryPaths( argv[ai]);
		std::vector<std::string>::const_iterator mi = modfiles.begin(), me = modfiles.end();
		for (; mi != me; ++mi)
		{
			std::cerr << "try '" << *mi << "'" << std::endl;
		}
		if (modloader->loadModule( argv[ai]))
		{
			std::cerr << "ok." << std::endl;
		}
		else
		{
			std::cerr << "failed." << std::endl;
		}
	}
	if (argc == 1)
	{
		std::cerr << "no modules loaded." << std::endl;
	}
	if (errorbuf->hasError())
	{
		std::cerr << "error testing module loader: " << errorbuf->fetchError() << std::endl;
		return -1;
	}
	return 0;
}



