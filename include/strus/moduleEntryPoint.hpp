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
#include "strus/versionAnalyzer.hpp"
#include "strus/versionStorage.hpp"
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
		Storage				///< module exporting objects for storage and query evaluation (moduleStorage)
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

	/// \brief Constructor for derived classes
	explicit ModuleEntryPoint( Type type_)
		:type(type_),modversion_minor(STRUS_MODULE_VERSION_MINOR)
	{
		const char* declaration_signature = STRUS_MODULE_SIGNATURE;
		//... signature contains major version number in it
		std::memset( signature, 0, sizeof(signature));
		std::memcpy( signature, declaration_signature, std::strlen( declaration_signature));
		std::memset( _reserved, 0, sizeof(_reserved));
		switch (type)
		{
			case Analyzer:
				compversion_major = STRUS_ANALYZER_VERSION_MAJOR;
				compversion_minor = STRUS_ANALYZER_VERSION_MINOR;
	
			case Storage:
				compversion_major = STRUS_STORAGE_VERSION_MAJOR;
				compversion_minor = STRUS_STORAGE_VERSION_MINOR;
	
			default:
				compversion_major = 0xFFFF;
				compversion_minor = 0xFFFF;
		}
	}

	static bool matchModuleVersion( const ModuleEntryPoint* entryPoint, int& errorcode);

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


const ModuleEntryPoint* loadModuleEntryPoint( const char* modfilename, ModuleEntryPoint::Status& status);

}//namespace
#endif

