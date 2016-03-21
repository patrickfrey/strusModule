/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/moduleEntryPoint.hpp"
#include "strus/private/dll_tags.hpp"
#include <cstring>
#include <vector>
#include <stdexcept>

#if defined(_WIN32)
#error Module Loader not ported to Windows, only loader for POSIX available
#else
#include <dlfcn.h>
#endif

using namespace strus;

DLL_PUBLIC bool ModuleEntryPoint::matchModuleVersion( const ModuleEntryPoint* entryPoint, int& errorcode)
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
		case ModuleEntryPoint::ErrorOpenModule: return "system error loading module";
		case ModuleEntryPoint::ErrorNoEntryPoint:return "no module entry point found";

		default: return "unknown error";
	};
}

static void initStatusMessage( ModuleEntryPoint::Status& status, const char* msg)
{
	std::size_t msglen = msg?std::strlen(msg):0;
	if (msglen >= sizeof(status.errormsg))
	{
		msglen = sizeof(status.errormsg)-1;
	}
	std::memcpy( status.errormsg, msg, msglen);
	status.errormsg[ msglen] = '\0';
}

DLL_PUBLIC const ModuleEntryPoint* strus::loadModuleEntryPoint( const char* modfilename, ModuleEntryPoint::Status& status)
{
	status.errorcode = 0;
	status.errormsg[0] = '\0';

	g_moduleHandleList.reserve(); //... do not run into (an unlikely) memory allocation error

	void* hnd = ::dlopen( modfilename, RTLD_NOW | RTLD_GLOBAL);
	if (!hnd)
	{
		status.errorcode = ModuleEntryPoint::ErrorOpenModule;
		initStatusMessage( status, ::dlerror());
		return 0;
	}
	ModuleEntryPoint* entryPoint = (ModuleEntryPoint*)dlsym( hnd, "entryPoint");
	if (!entryPoint)
	{
		status.errorcode = ModuleEntryPoint::ErrorNoEntryPoint;
		initStatusMessage( status, errorMessage( status.errorcode));
		return 0;
	}
	int errorcode = 0;
	if (ModuleEntryPoint::matchModuleVersion( entryPoint, errorcode))
	{
		status.errorcode = errorcode;
		initStatusMessage( status, errorMessage( status.errorcode));
		return 0;
	}
	g_moduleHandleList.addHandle( hnd);
	return entryPoint;
}

