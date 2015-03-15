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
#include "strus/private/fileio.hpp"
#include "utils.hpp"
#include <string>
#include <cstring>
#include <memory>
#include <iostream>

using namespace strus;

#define STRUS_LOWLEVEL_DEBUG

static void addModulePath_( std::vector<std::string>& paths, const char* pt)
{
	char const* cc = pt;
	char const* ee = std::strchr( cc, ':');
	for (; ee!=0; cc=ee+1,ee=std::strchr( cc, ':'))
	{
		paths.push_back( utils::trim( std::string( cc, ee)));
	}
	paths.push_back( utils::trim( std::string( cc)));
}

void ModuleLoader::addSystemModulePath()
{
	addModulePath_( m_paths, STRUS_MODULE_DIRECTORIES);
}

void ModuleLoader::addModulePath(const std::string& path)
{
	addModulePath_( m_paths, path.c_str());
}

void ModuleLoader::loadModule(const std::string& name)
{
	const ModuleEntryPoint* entryPoint;
	if (m_paths.empty())
	{
		std::vector<std::string> paths;
		addModulePath_( paths, STRUS_MODULE_DIRECTORIES);
		entryPoint = loadModuleAlt( name, paths);
	}
	else
	{
		entryPoint = loadModuleAlt( name, m_paths);
	}
	switch (entryPoint->type)
	{
		case ModuleEntryPoint::Analyzer:
			m_builder.addAnalyzerModule( reinterpret_cast<const AnalyzerModule*>( entryPoint));
			break;
		case ModuleEntryPoint::Storage:
			m_builder.addStorageModule( reinterpret_cast<const StorageModule*>( entryPoint));
			break;
	}
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
			return strus::loadModuleEntryPoint( modfilename.c_str());
		}
#ifdef STRUS_LOWLEVEL_DEBUG
		std::cerr << "try to load module '" << altmodfilename << "'" << std::endl;
#endif
		if (isFile( altmodfilename))
		{
			return strus::loadModuleEntryPoint( altmodfilename.c_str());
		}
	}
	throw std::runtime_error( std::string( "failed to load module '") + name + "' (not found)");
}



