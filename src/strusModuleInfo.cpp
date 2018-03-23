/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/lib/error.hpp"
#include "strus/lib/module.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/moduleLoaderInterface.hpp"
#include "strus/versionModule.hpp"
#include "strus/versionStorage.hpp"
#include "strus/versionAnalyzer.hpp"
#include "strus/versionTrace.hpp"
#include "internationalization.hpp"
#include "strus/moduleEntryPoint.hpp"
#include "strus/base/string_format.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/fileio.hpp"
#include <stdexcept>
#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <memory>

#undef STRUS_LOWLEVEL_DEBUG

static void printUsage()
{
	std::cout << "strusModuleInfo  { <modulepath> }" << std::endl;
	std::cout << "options:" << std::endl;
	std::cout << "-h|--help" << std::endl;
	std::cout << "    " << _TXT("Print this usage and do nothing else") << std::endl;
	std::cout << "-v|--version" << std::endl;
	std::cout << "    " << _TXT("Print the program version and do nothing else") << std::endl;
	std::cout << "<modulepath>  : " << _TXT("path of module to load.") << std::endl;
}

static strus::ErrorBufferInterface* g_errorBuffer = 0;	// error buffer

static bool printModuleVersion( const strus::ModuleEntryPoint* entryPoint, int& errorcode)
{
	errorcode = 0;
	std::cout << "module " << entryPoint->signature << "." << entryPoint->modversion_minor << std::endl;
	std::cout << "type ";
	switch (entryPoint->type)
	{
		case strus::ModuleEntryPoint::Analyzer:
			std::cout << "analyzer " << STRUS_ANALYZER_VERSION_STRING << std::endl;
			break;

		case strus::ModuleEntryPoint::Storage:
			std::cout << "storage " << STRUS_STORAGE_VERSION_STRING << std::endl;
			break;

		case strus::ModuleEntryPoint::Trace:
			std::cout << "trace " << STRUS_TRACE_VERSION_STRING << std::endl;
			break;

		default:
			std::cout << "unknown" << std::endl;
	}
	return true;
}

static void loadModule( const std::string& path)
{
	try
	{
		strus::ModuleEntryPoint::Status status;
		strus::ModuleEntryPoint::Handle hnd;
		if (loadModuleEntryPoint( path.c_str(), status, hnd, &printModuleVersion))
		{
			std::cout << _TXT("status ok") << std::endl;
		}
		else
		{
			std::cout << strus::string_format( _TXT("status error: %s"), status.errormsg) << std::endl;
		}
	}
	catch (const std::runtime_error& err)
	{
		std::cout << strus::string_format( _TXT("status error: %s"), err.what()) << std::endl;
	}
}


int main( int argc, const char* argv[])
{
	strus::local_ptr<strus::ErrorBufferInterface> errorBuffer( strus::createErrorBuffer_standard( 0, 2, NULL/*debug trace interface*/));
	if (!errorBuffer.get())
	{
		std::cerr << _TXT("failed to create error buffer") << std::endl;
		return -1;
	}
	g_errorBuffer = errorBuffer.get();

	try
	{
		bool doExit = false;
		std::vector<std::string> modulepathlist;

		// Parsing arguments:
		int argi = 1;
		for (; argi < argc; ++argi)
		{
			if (0==std::strcmp( argv[argi], "-h") || 0==std::strcmp( argv[argi], "--help"))
			{
				printUsage();
				doExit = true;
			}
			else if (0==std::strcmp( argv[argi], "-v") || 0==std::strcmp( argv[argi], "--version"))
			{
				std::cerr << strus::string_format( _TXT("strus storage version %s"), STRUS_MODULE_VERSION_STRING) << std::endl;
				doExit = true;
			}
			else if (argv[argi][0] == '-')
			{
				if (argv[argi][1] == '-')
				{
					++argi;
					break;
				}
				else
				{
					throw strus::runtime_error(_TXT("unknown option %s"), argv[ argi]);
				}
			}
			else
			{
				break;
			}
		}
		if (argc == 1)
		{
			std::cerr << _TXT("expected module path as argument") << std::endl;
			printUsage();
			return 0;
		}
		if (doExit)
		{
			return 0;
		}
		strus::local_ptr<strus::ModuleLoaderInterface> moduleLoader( strus::createModuleLoader( g_errorBuffer));
		if (!moduleLoader.get()) throw strus::runtime_error( "%s", _TXT("failed to create module loader"));

		for (; argi < argc; ++argi)
		{
			if (0==std::strchr( argv[ argi], strus::dirSeparator()))
			{
				std::cerr << strus::string_format(_TXT("search module %s"), argv[ argi]) << std::endl;
				std::vector<std::string> modfiles = moduleLoader->moduleLoadTryPaths( argv[argi]);
				std::vector<std::string>::const_iterator mi = modfiles.begin(), me = modfiles.end();
				for (; mi != me; ++mi)
				{
					std::cerr << "try path '" << *mi << "'" << std::endl;
				}
				if (modfiles.empty())
				{
					std::cout << strus::string_format( _TXT("status error: %s"), _TXT("not found")) << std::endl;
				}
				else if (strus::isFile( modfiles.back()))
				{
					std::cerr << strus::string_format(_TXT("load module %s"), modfiles.back().c_str()) << std::endl;
					loadModule( modfiles.back().c_str());
				}
				else
				{
					std::cout << strus::string_format( _TXT("status error: %s"), _TXT("not found")) << std::endl;
				}
			}
			else
			{
				std::cerr << strus::string_format(_TXT("load module %s"), argv[ argi]) << std::endl;
				loadModule( argv[ argi]);
			}
		}
		return 0;
	}
	catch (const std::exception& e)
	{
		const char* errormsg = g_errorBuffer?g_errorBuffer->fetchError():0;
		if (errormsg)
		{
			std::cerr << e.what() << ": " << errormsg << std::endl;
		}
		else
		{
			std::cerr << e.what() << std::endl;
		}
	}
	std::cerr << _TXT("terminated") << std::endl;
	return -1;
}

