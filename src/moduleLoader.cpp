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
#include "moduleLoader.hpp"
#include "moduleDirectory.hpp"
#include "strus/moduleEntryPoint.hpp"
#include "strus/storageModule.hpp"
#include "strus/analyzerModule.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/analyzerErrorBufferInterface.hpp"
#include "storageObjectBuilder.hpp"
#include "analyzerObjectBuilder.hpp"
#include "strus/private/fileio.hpp"
#include "strus/private/snprintf.h"
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

class AnalyzerErrorBuffer
	:public AnalyzerErrorBufferInterface
{
public:
	explicit AnalyzerErrorBuffer( ErrorBufferInterface* errorhnd_)
		:m_errorhnd(errorhnd_){}

	virtual void report( const char* format, ...) const
	{
		va_list ap;
		va_start(ap, format);
		char msgbuf[ 1024];
		strus_vsnprintf( msgbuf, sizeof(msgbuf), format, ap);
		va_end(ap);
		m_errorhnd->report( "%s", msgbuf);
	}

	virtual void explain( const char* format) const
	{
		m_errorhnd->explain( format);
	}

	virtual const char* fetchError()
	{
		return m_errorhnd->fetchError();
	}

	virtual bool hasError() const
	{
		return m_errorhnd->hasError();
	}
	
private:
	ErrorBufferInterface* m_errorhnd;
};

ModuleLoader::ModuleLoader( ErrorBufferInterface* errorhnd_)
	:m_peermsgproc_enabled(false),m_errorhnd(errorhnd_),m_errorhnd_analyzer(0)
{
	m_errorhnd_analyzer = new AnalyzerErrorBuffer( m_errorhnd);
}

ModuleLoader::~ModuleLoader()
{
	if (m_errorhnd_analyzer) delete m_errorhnd_analyzer;
}

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

void ModuleLoader::enablePeerMessageProcessor( const std::string& name)
{
	try
	{
		m_peermsgproc = name;
		m_peermsgproc_enabled = true;
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
		std::auto_ptr<StorageObjectBuilder> builder( new StorageObjectBuilder( m_peermsgproc_enabled?m_peermsgproc.c_str():0, m_errorhnd));
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
		std::auto_ptr<AnalyzerObjectBuilder> builder( new AnalyzerObjectBuilder( m_errorhnd_analyzer));
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



