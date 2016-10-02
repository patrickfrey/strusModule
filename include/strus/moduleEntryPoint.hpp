/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Header with object types needed for declaration of a strus module
/// \file moduleEntryPoint.hpp
#ifndef _STRUS_MODULE_HEADER_HPP_INCLUDED
#define _STRUS_MODULE_HEADER_HPP_INCLUDED
#include "strus/versionModule.hpp"
#include <cstring>

/// \brief strus toplevel namespace
namespace strus {

/// \brief Entry point of a module with signature and version info to verify correctness of the dynamic linking
struct ModuleEntryPoint
{
	/// \brief The module types supported
	enum Type
	{
		Analyzer,			///< module exporting objects for document and query analysis (moduleAnalyzer)
		Storage,			///< module exporting objects for storage and query evaluation (moduleStorage)
		Trace				///< module exporting objects for method call trace generation (moduleTrace)
	};
	enum
	{
		ErrorNone=0,			///< no error
		ErrorUnknownModuleType=1,	///< the 'type' field has an unknown value
		ErrorSignature=11,		///< the signature (composition of a string and the major module version number) does not match
		ErrorModMinorVersion=12,	///< the loaded module minor version is higher than the one of the module loader. This means that the module potentially implementents features that are not known to the module loader. The loading of such a module is thus refused.
		ErrorCompMajorVersion=21,	///< the components implemented in the module have a major version number than expected
		ErrorCompMinorVersion=22,	///< the components minor version is smaller than required. The module may not implement the objects loaded as required. Thus the loading of the module is refused.
		ErrorOpenModule=31,
		ErrorNoEntryPoint=32
	};

	char signature[ 8];			///< signature of the module (string + major version)
	Type type;				///< type of the module
	unsigned short modversion_minor;	///< minor version of the module
	unsigned short compversion_major;	///< major version of components in the module
	unsigned short compversion_minor;	///< minor version of components in the module
	unsigned int _reserved[6];		///< reserved for future use
	const char* version_3rdparty;		///< 3rd party version info
	const char* license_3rdparty;		///< 3rd party license text

	/// \brief Constructor for derived classes
	explicit ModuleEntryPoint( Type type_, unsigned short version_major, unsigned short version_minor, const char* version_3rdparty_=0, const char* license_3rdparty_=0)
		:type(type_),modversion_minor(STRUS_MODULE_VERSION_MINOR),version_3rdparty(version_3rdparty_),license_3rdparty(license_3rdparty_)
	{
		const char* declaration_signature = STRUS_MODULE_SIGNATURE;
		//... signature contains major version number in it
		std::memset( signature, 0, sizeof(signature));
		std::memcpy( signature, declaration_signature, std::strlen( declaration_signature));
		std::memset( _reserved, 0, sizeof(_reserved));
		compversion_major = version_major;
		compversion_minor = version_minor;
	}

	struct Status
	{
		Status()
			:errorcode(0)
		{
			errormsg[0] = '\0';
		}

		bool ok() const		{return errorcode==0;}
	
		int errorcode;
		char errormsg[ 256];
	};
};

typedef bool (*MatchModuleVersionFunc)( const ModuleEntryPoint* entryPoint, int& errorcode);
const ModuleEntryPoint* loadModuleEntryPoint( const char* modfilename, ModuleEntryPoint::Status& status, MatchModuleVersionFunc);

}//namespace
#endif

