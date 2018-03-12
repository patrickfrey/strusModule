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
#include "strus/versionStorage.hpp"
#include "strus/analyzerModule.hpp"
#include "strus/versionAnalyzer.hpp"
#include "strus/traceModule.hpp"
#include "strus/versionTrace.hpp"
#include "strus/errorBufferInterface.hpp"
#include "strus/storageInterface.hpp"
#include "strus/databaseInterface.hpp"
#include "strus/lib/traceobj.hpp"
#include "strus/lib/traceproc_std.hpp"
#include "storageObjectBuilder.hpp"
#include "analyzerObjectBuilder.hpp"
#include "strus/base/fileio.hpp"
#include "strus/base/env.hpp"
#include "strus/base/configParser.hpp"
#include "strus/base/local_ptr.hpp"
#include "strus/base/string_conv.hpp"
#include "strus/traceLoggerInterface.hpp"
#include "errorUtils.hpp"
#include "internationalization.hpp"
#include <string>
#include <cstring>
#include <memory>
#include <iostream>
#include <stdarg.h>

using namespace strus;

#undef STRUS_LOWLEVEL_DEBUG
#define ENV_STRUS_MODULE_PATH "STRUS_MODULE_PATH"

ModuleLoader::ModuleLoader( ErrorBufferInterface* errorhnd_)
	:m_errorhnd(errorhnd_)
{}

ModuleLoader::~ModuleLoader()
{
	std::vector<ModuleEntryPoint::Handle>::iterator hi = m_handleList.begin(), he = m_handleList.end();
	for (; hi != he; ++hi)
	{
		ModuleEntryPoint::closeHandle( *hi);
	}
}

static void addPath_( std::vector<std::string>& paths, const char* pt)
{
	char const* cc = pt;
	char const* ee = std::strchr( cc, STRUS_MODULE_PATHSEP);
	for (; ee!=0; cc=ee+1,ee=std::strchr( cc, STRUS_MODULE_PATHSEP))
	{
		paths.push_back( string_conv::trim( std::string( cc, ee)));
	}
	paths.push_back( string_conv::trim( std::string( cc)));
}

void ModuleLoader::addSystemModulePath()
{
	try
	{
		addPath_( m_modulePaths, STRUS_MODULE_DIRECTORIES);
	}
	catch (const std::bad_alloc&)
	{
		m_errorhnd->report( ErrorCodeOutOfMem, _TXT("out of memory in module loader"));
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
		m_errorhnd->report( ErrorCodeOutOfMem, _TXT("out of memory in module loader"));
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
		m_errorhnd->report( ErrorCodeOutOfMem, _TXT("out of memory in module loader"));
	}
}

void ModuleLoader::defineWorkingDirectory( const std::string& path)
{
	try
	{
		m_workdir = path;
	}
	catch (const std::bad_alloc&)
	{
		m_errorhnd->report( ErrorCodeOutOfMem, _TXT("out of memory in module loader"));
	}
}

const ModuleEntryPoint* ModuleLoader::searchAndLoadEntryPoint( const std::string& name, std::vector<std::string>& paths_tried)
{
	const ModuleEntryPoint* entryPoint = loadModuleAlt( name, m_modulePaths, paths_tried);
	if (!entryPoint)
	{
		std::vector<std::string> paths;
		int ec = getenv_list( ENV_STRUS_MODULE_PATH, separatorPathList(), paths);
		if (ec)
		{
			m_errorhnd->report( ec, _TXT("failed to read environment variable %s in module loader: %s"), ENV_STRUS_MODULE_PATH, ::strerror(ec));
			return 0;
		}
		if (m_modulePaths.empty())
		{
			addPath_( paths, STRUS_MODULE_DIRECTORIES);
		}
		entryPoint = loadModuleAlt( name, paths, paths_tried);
	}
	return entryPoint;
}

bool ModuleLoader::loadModule(const std::string& name)
{
	try
	{
		if (hasUpdirReference( name))
		{
			m_errorhnd->report( ErrorCodeInvalidFilePath, _TXT("tried to load module with upper directory reference in the module name"));
			return false;
		}
		std::vector<std::string> paths_tried;
		const ModuleEntryPoint* entryPoint = searchAndLoadEntryPoint( name, paths_tried);
		if (!entryPoint)
		{
			m_errorhnd->report( ErrorCodeLoadModuleFailed, _TXT("failed to load module '%s': "), name.c_str());
			return false;
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
				if (entryPoint->license_3rdparty)
				{
					m_license_3rdparty_ar.push_back( entryPoint->license_3rdparty);
				}
				if (entryPoint->version_3rdparty)
				{
					m_version_3rdparty_ar.push_back( entryPoint->version_3rdparty);
				}
				std::string pname;
				int ec = strus::getFileName( name, pname, false);
				if (ec)
				{
					m_errorhnd->report( ec, "%s", ::strerror(ec));
					return false;
				}
				m_modules.push_back( pname);
				return true;
			}
			catch (const std::bad_alloc&)
			{
				m_errorhnd->report( ErrorCodeOutOfMem, _TXT("out of memory in module loader"));
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

std::vector<std::string> ModuleLoader::moduleLoadTryPaths( const std::string& name)
{
	try
	{
		std::vector<std::string> rt;
		searchAndLoadEntryPoint( name, rt);
		return rt;
	}
	CATCH_ERROR_MAP_RETURN( _TXT("error seeking for module (moduleLoadTryPaths): %s"), *m_errorhnd, std::vector<std::string>());
}

StorageObjectBuilderInterface* ModuleLoader::createStorageObjectBuilder() const
{
	try
	{
		strus::local_ptr<StorageObjectBuilder> builder( new StorageObjectBuilder( m_workdir, m_errorhnd));
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
		strus::local_ptr<AnalyzerObjectBuilder> builder( new AnalyzerObjectBuilder( m_errorhnd));
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
			if (strus::caseInsensitiveEquals( loggerName, car[ci].title))
			{
				return car->create( config, m_errorhnd);
			}
		}
	}
	if (strus::caseInsensitiveEquals( loggerName, "dump"))
	{
		return createTraceLogger_dump( config, m_errorhnd);
	}
	else if (strus::caseInsensitiveEquals( loggerName, "json"))
	{
		return createTraceLogger_json( config, m_errorhnd);
	}
	else if (strus::caseInsensitiveEquals( loggerName, "breakpoint"))
	{
		return createTraceLogger_breakpoint( config, m_errorhnd);
	}
	else if (strus::caseInsensitiveEquals( loggerName, "count"))
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

std::vector<std::string> ModuleLoader::get3rdPartyLicenseTexts() const
{
	return m_license_3rdparty_ar;
	
}
std::vector<std::string> ModuleLoader::get3rdPartyVersionTexts() const
{
	return m_version_3rdparty_ar;
}

static bool matchModuleVersion( const ModuleEntryPoint* entryPoint, int& errorcode)
{
	const char* loader_signature = STRUS_MODULE_SIGNATURE;
	//... signature contains major version number in it
	if (std::strcmp( entryPoint->signature, loader_signature) != 0)
	{
		errorcode = ModuleEntryPoint::ErrorSignature;
		return false;
	}
	unsigned short expected_modversion_minor = STRUS_MODULE_VERSION_MINOR;
	if (entryPoint->modversion_minor > expected_modversion_minor)
	{
		errorcode = ModuleEntryPoint::ErrorModMinorVersion;
		return false;
	}
	unsigned short expected_compversion_major = 0;
	unsigned short expected_compversion_minor = 0;
	switch (entryPoint->type)
	{
		case ModuleEntryPoint::Analyzer:
			expected_compversion_major = STRUS_ANALYZER_VERSION_MAJOR;
			expected_compversion_minor = STRUS_ANALYZER_VERSION_MINOR;
			break;

		case ModuleEntryPoint::Storage:
			expected_compversion_major = STRUS_STORAGE_VERSION_MAJOR;
			expected_compversion_minor = STRUS_STORAGE_VERSION_MINOR;
			break;

		case ModuleEntryPoint::Trace:
			expected_compversion_major = STRUS_TRACE_VERSION_MAJOR;
			expected_compversion_minor = STRUS_TRACE_VERSION_MINOR;
			break;

		default:
			errorcode = ModuleEntryPoint::ErrorUnknownModuleType;
			return false;
	}
	if (entryPoint->compversion_major != expected_compversion_major)
	{
		errorcode = ModuleEntryPoint::ErrorCompMajorVersion;
		return false;
	}
	if (entryPoint->compversion_minor < expected_compversion_minor)
	{
		errorcode = ModuleEntryPoint::ErrorCompMinorVersion;
		return false;
	}
	return true;
}

const ModuleEntryPoint* ModuleLoader::loadModuleAlt(
		const std::string& name,
		const std::vector<std::string>& paths,
		std::vector<std::string>& paths_tried)
{
	std::vector<std::string>::const_iterator pi = paths.begin(), pe = paths.end();
	for (; pi != pe; ++pi)
	{
		std::string modfilename;
		if (stringStartsWith( name, "modstrus_"))
		{
			modfilename = strus::joinFilePath( *pi, name);
		}
		else if (stringStartsWith( name, "strus_"))
		{
			modfilename = strus::joinFilePath( *pi, "mod" + name);
		}
		else
		{
			modfilename = strus::joinFilePath( *pi, "modstrus_" + name);
		}
		if (!strus::caseInsensitiveEquals(
			modfilename.c_str() + modfilename.size() - std::strlen( STRUS_MODULE_EXTENSION),
			STRUS_MODULE_EXTENSION))
		{
			modfilename.append( STRUS_MODULE_EXTENSION);
		}
		const ModuleEntryPoint* entrypoint;
		paths_tried.push_back( modfilename);
		if (!!(entrypoint = tryLoadPathAsModule( modfilename))) return entrypoint;
	}
	return 0;
}

const ModuleEntryPoint* ModuleLoader::tryLoadPathAsModule( const std::string& modpath)
{
#ifdef STRUS_LOWLEVEL_DEBUG
	std::cerr << "try to load module '" << modpath << "'" << std::endl;
#endif
	if (isFile( modpath))
	{
		ModuleEntryPoint::Status status;
		ModuleEntryPoint::Handle modhnd = NULL;
		const ModuleEntryPoint* entrypoint = strus::loadModuleEntryPoint( modpath.c_str(), status, modhnd, &matchModuleVersion);
		if (!entrypoint)
		{
			m_errorhnd->report( ErrorCodeLoadModuleFailed, _TXT("error loading module '%s': %s"), modpath.c_str(), status.errormsg);
		}
		else
		{
			m_handleList.push_back( modhnd);
		}
		return entrypoint;
	}
	else
	{
		return NULL;
	}
}


