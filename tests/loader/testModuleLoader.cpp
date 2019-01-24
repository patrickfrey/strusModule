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
#include "strus/debugTraceInterface.hpp"
#include "testModuleDirectory.hpp"
#include "strus/base/local_ptr.hpp"
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

static void printUsage()
{
	std::cerr << "testModuleLoader [options] <modulename>" << std::endl;
	std::cerr << "Options:" << std::endl;
	std::cerr << "       -G|--debug <ID>    :enable debug for <ID>" << std::endl;
	std::cerr << "       -h|--help          :print this usage" << std::endl;
}

int main( int argc, const char** argv)
{
	strus::DebugTraceInterface* dbgtrace = strus::createDebugTrace_standard( 2);
	strus::local_ptr<strus::ErrorBufferInterface> errorbuf( strus::createErrorBuffer_standard( stderr, 1, dbgtrace));
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

	int argi = 1;
	for (; argi < argc && argv[argi][0] == '-'; ++argi)
	{
		if (0==std::strcmp( argv[argi], "--debug") || 0==std::strcmp( argv[argi], "-G"))
		{
			if (!argv[++argi]) throw std::runtime_error( "missing argument for option --debug / -G");
			if (!dbgtrace->enable( argv[argi])) throw std::runtime_error( "failed to enable debug");
		}
		else if (0==std::strcmp( argv[argi], "--help") || 0==std::strcmp( argv[argi], "-h"))
		{
			printUsage();
			exit( 0);
		}
		else if (0==std::strcmp( argv[argi], "--"))
		{
			argi++;
			break;
		}
		else
		{
			std::cerr << "Unknown option " << argv[argi] << std::endl;
			printUsage();
			exit( 1);
		}
	}
	if (argi == argc)
	{
		std::cerr << "Too few arguments" << std::endl;
		printUsage();
		exit( 1);
	}
	int ai = argi, ae = argc;
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



