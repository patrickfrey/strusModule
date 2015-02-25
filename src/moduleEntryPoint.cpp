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

