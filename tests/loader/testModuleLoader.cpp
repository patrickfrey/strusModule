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
#include "strus/lib/module.hpp"
#include "strus/lib/error.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/errorBufferInterface.hpp"
#include "testModuleDirectory.hpp"
#include <memory>
#include <string>
#include <stdexcept>
#include <iostream>
#include <cstdio>

int main( int argc, const char** argv)
{
	try
	{
		std::auto_ptr<strus::ErrorBufferInterface> errorbuf( strus::createErrorBuffer_standard( stderr));
		std::auto_ptr<strus::ModuleLoaderInterface> modloader( strus::createModuleLoader( errorbuf.get()));
		std::cerr << "setting load module path to '" << STRUS_TEST_MODULE_DIRECTORY << "'" << std::endl;
		modloader->addModulePath( STRUS_TEST_MODULE_DIRECTORY);
	
		int ai = 1, ae = argc;
		for (; ai != ae; ++ai)
		{
			std::cerr << "loading module '" << argv[ai] << "'" << std::endl;
			modloader->loadModule( argv[ai]);
		}
		if (argc == 1)
		{
			std::cerr << "no modules loaded." << std::endl;
		}
		return 0;
	}
	catch (const std::runtime_error& err)
	{
		std::cerr << "ERROR " << err.what() << std::endl;
		return 1;
	}
}



