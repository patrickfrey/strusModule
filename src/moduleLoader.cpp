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
#include "storageObjectBuilder.hpp"
#include "analyzerObjectBuilder.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/snprintf.h"
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
	:m_statsproc_enabled(false),m_errorhnd(errorhnd_)
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

void ModuleLoader::defineStatisticsProcessor( const std::string& name)
{
	try
	{
		m_statsproc = name;
		m_statsproc_enabled = true;
	}
	catch (const std::bad_alloc&)
	{
		m_errorhnd->report(_TXT("out of memory in module loader"));
	}
}

bool ModuleLoader::loadModule(const std::string& name)
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

StorageObjectBuilderInterface* ModuleLoader::createStorageObjectBuilder() const
{
	try
	{
		std::auto_ptr<StorageObjectBuilder> builder( new StorageObjectBuilder( m_statsproc_enabled?m_statsproc.c_str():0, m_errorhnd));
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
				m_errorhnd->report(_TXT("error loading module '%s': "), modfilename.c_str(), status.errormsg);
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
				m_errorhnd->report(_TXT("error loading module '%s': "), altmodfilename.c_str(), status.errormsg);
			}
			return entrypoint;
		}
	}
	m_errorhnd->report(_TXT("failed to find module '%s': "), name.c_str());
	return 0;
}



