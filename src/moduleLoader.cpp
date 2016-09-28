/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "moduleLoader.hpp"
#include "moduleDirectory.hpp"
#include "strus/moduleEntryPoint.hpp"
#include "strus/storageModule.hpp"
#include "strus/analyzerModule.hpp"
#include "strus/traceModule.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/storageInterface.hpp"
#include "strus/databaseInterface.hpp"
#include "strus/lib/traceobj.hpp"
#include "strus/lib/traceproc_std.hpp"
#include "storageObjectBuilder.hpp"
#include "analyzerObjectBuilder.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/configParser.hpp"
#include "strus/traceLoggerInterface.hpp"
#include "utils.hpp"
#include "errorUtils.hpp"
#include "internationalization.hpp"
#include <string>
#include <cstring>
#include <memory>
#include <iostream>
#include <stdarg.h>

using namespace strus;

#undef STRUS_LOWLEVEL_DEBUG

ModuleLoader::ModuleLoader( ErrorBufferInterface* errorhnd_)
	:m_errorhnd(errorhnd_)
{}

ModuleLoader::~ModuleLoader()
{}

static void addPath_( std::vector<std::string>& paths, const char* pt)
{
	char const* cc = pt;
	char const* ee = std::strchr( cc, STRUS_MODULE_PATHSEP);
	for (; ee!=0; cc=ee+1,ee=std::strchr( cc, STRUS_MODULE_PATHSEP))
	{
		paths.push_back( utils::trim( std::string( cc, ee)));
	}
	paths.push_back( utils::trim( std::string( cc)));
}

void ModuleLoader::addSystemModulePath()
{
	try
	{
		addPath_( m_modulePaths, STRUS_MODULE_DIRECTORIES);
	}
	catch (const std::bad_alloc&)
	{
		m_errorhnd->report(_TXT("out of memory in module loader"));
	}
}

void ModuleLoader::addModulePath(const std::string& path)
{
	try
	{
		addPath_( m_modulePaths, path.c_str());
	}
	catch (const std::bad_alloc&)
	{
		m_errorhnd->report(_TXT("out of memory in module loader"));
	}
}

void ModuleLoader::addResourcePath( const std::string& path)
{
	try
	{
		addPath_( m_resourcePaths, path.c_str());
	}
	catch (const std::bad_alloc&)
	{
		m_errorhnd->report(_TXT("out of memory in module loader"));
	}
}

bool ModuleLoader::loadModule(const std::string& name)
{
	try
	{
		const ModuleEntryPoint* entryPoint;
		if (m_modulePaths.empty())
		{
			std::vector<std::string> paths;
			try
			{
				addPath_( paths, STRUS_MODULE_DIRECTORIES);
			}
			catch (const std::bad_alloc&)
			{
				m_errorhnd->report(_TXT("out of memory in module loader"));
				return false;
			}
			entryPoint = loadModuleAlt( name, paths);
		}
		else
		{
			entryPoint = loadModuleAlt( name, m_modulePaths);
		}
		if (entryPoint)
		{
			try
			{
				switch (entryPoint->type)
				{
					case ModuleEntryPoint::Analyzer:
						m_analyzerModules.push_back( reinterpret_cast<const AnalyzerModule*>( entryPoint));
						break;
					case ModuleEntryPoint::Storage:
						m_storageModules.push_back( reinterpret_cast<const StorageModule*>( entryPoint));
						break;
					case ModuleEntryPoint::Trace:
						m_traceModules.push_back( reinterpret_cast<const TraceModule*>( entryPoint));
						break;
				}
				return true;
			}
			catch (const std::bad_alloc&)
			{
				m_errorhnd->report(_TXT("out of memory in module loader"));
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error loading module: %s"), *m_errorhnd, 0);
}

StorageObjectBuilderInterface* ModuleLoader::createStorageObjectBuilder() const
{
	try
	{
		std::auto_ptr<StorageObjectBuilder> builder( new StorageObjectBuilder( m_errorhnd));
		std::vector<const StorageModule*>::const_iterator
			mi = m_storageModules.begin(), me = m_storageModules.end();
		for (; mi != me; ++mi)
		{
			builder->addStorageModule( *mi);
		}
		return builder.release();
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error creating storage object builder: %s"), *m_errorhnd, 0);
}

AnalyzerObjectBuilderInterface* ModuleLoader::createAnalyzerObjectBuilder() const
{
	try
	{
		std::auto_ptr<AnalyzerObjectBuilder> builder( new AnalyzerObjectBuilder( m_errorhnd));
		std::vector<std::string>::const_iterator
			pi = m_resourcePaths.begin(), pe = m_resourcePaths.end();
		for (; pi != pe; ++pi)
		{
			builder->addResourcePath( *pi);
		}
		std::vector<const AnalyzerModule*>::const_iterator
			mi = m_analyzerModules.begin(), me = m_analyzerModules.end();
		for (; mi != me; ++mi)
		{
			builder->addAnalyzerModule( *mi);
		}
		return builder.release();
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error creating storage object builder: %s"), *m_errorhnd, 0);
}


TraceLoggerInterface* ModuleLoader::createTraceLogger( const std::string& loggerName, const std::string& config) const
{
	std::vector<const TraceModule*>::const_iterator mi = m_traceModules.begin(), me = m_traceModules.end();
	for (; mi != me; ++mi)
	{
		const TraceLoggerConstructor* car = (*mi)->traceLoggerConstructors;
		std::size_t ci = 0;
		for (; car[ci].title; ++ci)
		{
			if (utils::caseInsensitiveEquals( loggerName, car[ci].title))
			{
				return car->create( config, m_errorhnd);
			}
		}
	}
	if (utils::caseInsensitiveEquals( loggerName, "dump"))
	{
		return createTraceLogger_dump( config, m_errorhnd);
	}
	else if (utils::caseInsensitiveEquals( loggerName, "json"))
	{
		return createTraceLogger_json( config, m_errorhnd);
	}
	else if (utils::caseInsensitiveEquals( loggerName, "breakpoint"))
	{
		return createTraceLogger_breakpoint( config, m_errorhnd);
	}
	else if (utils::caseInsensitiveEquals( loggerName, "count"))
	{
		return createTraceLogger_count( config, m_errorhnd);
	}
	else
	{
		throw strus::runtime_error(_TXT("unknown trace logger '%s' (did you load its module)"), loggerName.c_str());
	}
}

TraceObjectBuilderInterface* ModuleLoader::createTraceObjectBuilder( const std::string& config_) const
{
	try
	{
		std::string config( config_);
		std::string modulename;
		if (!extractStringFromConfigString( modulename, config, "log", m_errorhnd))
		{
			throw strus::runtime_error(_TXT("undefined '%s' in config"), "log");
		}
		TraceLoggerInterface* tracelog = createTraceLogger( modulename, config);
		if (!tracelog)
		{
			throw strus::runtime_error( _TXT( "could not load trace processor '%s'"), modulename.c_str());
		}
		TraceObjectBuilderInterface* rt = traceCreateObjectBuilder( tracelog, m_errorhnd);
		if (!rt) delete tracelog;
		return rt;
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error creating storage object builder: %s"), *m_errorhnd, 0);
}

const ModuleEntryPoint* ModuleLoader::loadModuleAlt(
		const std::string& name,
		const std::vector<std::string>& paths)
{
	std::vector<std::string>::const_iterator pi = paths.begin(), pe = paths.end();
	for (; pi != pe; ++pi)
	{
		std::string modfilename( *pi + dirSeparator() + name);
		std::string altmodfilename( *pi + dirSeparator() + "modstrus_" + name);
		if (!utils::caseInsensitiveEquals(
			modfilename.c_str() + modfilename.size() - std::strlen( STRUS_MODULE_EXTENSION),
			STRUS_MODULE_EXTENSION))
		{
			modfilename.append( STRUS_MODULE_EXTENSION);
			altmodfilename.append( STRUS_MODULE_EXTENSION);
		}
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << "try to load module '" << modfilename << "'" << std::endl;
#endif
		if (isFile( modfilename))
		{
			ModuleEntryPoint::Status status;
			const ModuleEntryPoint* entrypoint = strus::loadModuleEntryPoint( modfilename.c_str(), status);
			if (!entrypoint)
			{
				m_errorhnd->report(_TXT("error loading module '%s': %s"), modfilename.c_str(), status.errormsg);
			}
			return entrypoint;
		}
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << "try to load module '" << altmodfilename << "'" << std::endl;
#endif
		if (isFile( altmodfilename))
		{
			ModuleEntryPoint::Status status;
			const ModuleEntryPoint* entrypoint = strus::loadModuleEntryPoint( altmodfilename.c_str(), status);
			if (!entrypoint)
			{
				m_errorhnd->report(_TXT("error loading module '%s': %s"), altmodfilename.c_str(), status.errormsg);
			}
			return entrypoint;
		}
	}
	m_errorhnd->report(_TXT("failed to find module '%s': "), name.c_str());
	return 0;
}



