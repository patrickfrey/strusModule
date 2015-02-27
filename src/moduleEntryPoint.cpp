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
#include "strus/moduleEntryPoint.hpp"
#include <cstring>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>

#if defined(_WIN32)
#error Module Loader not ported to Windows, only loader for POSIX available
#else
#include <dlfcn.h>
#endif

using namespace strus;

bool ModuleEntryPoint::matchModuleVersion( const ModuleEntryPoint* entryPoint, int& errorcode)
{
	const char* loader_signature = STRUS_MODULE_SIGNATURE;
	//... signature contains major version number in it
	if (std::strcmp( entryPoint->signature, loader_signature) != 0)
	{
		errorcode = ErrorSignature;
		return false;
	}
	unsigned short expected_modversion_minor = STRUS_MODULE_VERSION_MINOR;
	if (entryPoint->modversion_minor > expected_modversion_minor)
	{
		errorcode = ErrorModMinorVersion;
		return false;
	}
	unsigned short expected_compversion_major = 0;
	unsigned short expected_compversion_minor = 0;
	switch (entryPoint->type)
	{
		case Analyzer:
			expected_compversion_major = STRUS_ANALYZER_VERSION_MAJOR;
			expected_compversion_minor = STRUS_ANALYZER_VERSION_MINOR;

		case Storage:
			expected_compversion_major = STRUS_STORAGE_VERSION_MAJOR;
			expected_compversion_minor = STRUS_STORAGE_VERSION_MINOR;

		default:
			errorcode = ErrorUnknownModuleType;
			return false;
	}
	if (entryPoint->compversion_major != expected_compversion_major)
	{
		errorcode = ErrorCompMajorVersion;
		return false;
	}
	if (entryPoint->compversion_minor < expected_compversion_minor)
	{
		errorcode = ErrorCompMinorVersion;
		return false;
	}
	return true;
}


namespace {
class ModuleHandleList
{
public:
	~ModuleHandleList()
	{
		std::vector<void*>::const_iterator
			hi = m_handleList.begin(), he = m_handleList.end();
		for (; hi != he; ++hi)
		{
			::dlclose( *hi);
		}
	}

	void addHandle( void* handle)
	{
		m_handleList.push_back( handle);
	}

	void reserve()
	{
		m_handleList.reserve( m_handleList.size()+1);
	}

private:
	std::vector<void*> m_handleList;

};
}// anonymous namespace

static ModuleHandleList g_moduleHandleList;

static const char* errorMessage( int error)
{
	switch (error)
	{
		case ModuleEntryPoint::ErrorUnknownModuleType: return "module type unknown";
		case ModuleEntryPoint::ErrorSignature: return "module signature mismatch";
		case ModuleEntryPoint::ErrorModMinorVersion: return "module newer in minor version than module loader, it is compatible but may contain objects that cannot be loaded";
		case ModuleEntryPoint::ErrorCompMajorVersion: return "loaded objects major version mismatch";
		case ModuleEntryPoint::ErrorCompMinorVersion: return "loaded objects minor version smaller than required by the loader";
		default: return "unknown error";
	};
	
}


const ModuleEntryPoint* strus::loadModuleEntryPoint( const char* modfilename)
{
	g_moduleHandleList.reserve(); //... do not run into (an unlikely) memory allocation error

	void* hnd = ::dlopen( modfilename, RTLD_NOW | RTLD_LOCAL);
	if (!hnd)
	{
		std::ostringstream msg;
		msg << "error opening module " << modfilename << " (" << ::dlerror() << ")";
		throw std::runtime_error( std::string(msg.str()));
	}
	ModuleEntryPoint* entryPoint = (ModuleEntryPoint*)dlsym( hnd, "entryPoint");
	if (!entryPoint)
	{
		std::ostringstream msg;
		msg << "no entry point found in module " << modfilename << " (" << ::dlerror() << ")";
		::dlclose( hnd);
		throw std::runtime_error( std::string( msg.str()));
	}
	int errorcode = 0;
	if (ModuleEntryPoint::matchModuleVersion( entryPoint, errorcode))
	{
		std::ostringstream msg;
		msg << "incompatible content in the module " << modfilename << " (" << errorMessage( errorcode) << ")";
		::dlclose( hnd);
		throw std::runtime_error( std::string( msg.str()));
		
	}
	g_moduleHandleList.addHandle( hnd);
	return entryPoint;
}
