/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/moduleEntryPoint.hpp"
#include "strus/base/dll_tags.hpp"
#include "internationalization.hpp"
#include <cstring>
#include <vector>
#include <stdexcept>

#if defined(_WIN32)
#error Module Loader not ported to Windows, only loader for POSIX available
#else
#include <dlfcn.h>
#endif

using namespace strus;

DLL_PUBLIC void ModuleEntryPoint::closeHandle( Handle& hnd)
{
	//[+] PH:HACK: Disabled because of segfaults in bindings caused by this: if (hnd) ::dlclose( hnd);
}

static const char* errorMessage( int error)
{
	switch (error)
	{
		case ModuleEntryPoint::ErrorUnknownModuleType: return _TXT("module type unknown");
		case ModuleEntryPoint::ErrorSignature: return _TXT("module signature mismatch");
		case ModuleEntryPoint::ErrorModMinorVersion: return _TXT("module newer in minor version than module loader, it is compatible but may contain objects that cannot be loaded");
		case ModuleEntryPoint::ErrorCompMajorVersion: return _TXT("loaded objects major version mismatch");
		case ModuleEntryPoint::ErrorCompMinorVersion: return _TXT("loaded objects minor version smaller than required by the loader");
		case ModuleEntryPoint::ErrorOpenModule: return _TXT("system error loading module");
		case ModuleEntryPoint::ErrorNoEntryPoint:return _TXT("no module entry point found");

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

DLL_PUBLIC const ModuleEntryPoint* strus::loadModuleEntryPoint( const char* modfilename, ModuleEntryPoint::Status& status, ModuleEntryPoint::Handle& hnd, MatchModuleVersionFunc matchVersion)
{
	status.errorcode = 0;
	status.errormsg[0] = '\0';

	hnd = ::dlopen( modfilename, RTLD_NOW | RTLD_LOCAL);
	if (!hnd)
	{
		status.errorcode = ModuleEntryPoint::ErrorOpenModule;
		initStatusMessage( status, ::dlerror());
		return 0;
	}
	ModuleEntryPoint* entryPoint = (ModuleEntryPoint*)::dlsym( hnd, "entryPoint");
	if (!entryPoint)
	{
		::dlclose( hnd);
		hnd = 0;
		status.errorcode = ModuleEntryPoint::ErrorNoEntryPoint;
		initStatusMessage( status, errorMessage( status.errorcode));
		return 0;
	}
	int errorcode = 0;
	if (!(*matchVersion)( entryPoint, errorcode))
	{
		::dlclose( hnd);
		hnd = 0;
		status.errorcode = errorcode;
		initStatusMessage( status, errorMessage( status.errorcode));
		return 0;
	}
	return entryPoint;
}

