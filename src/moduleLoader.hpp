/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#ifndef _STRUS_MODULE_LOADER_HPP_INCLUDED
#define _STRUS_MODULE_LOADER_HPP_INCLUDED
#include "strus/moduleLoaderInterface.hpp"
#include "strus/moduleEntryPoint.hpp"
#include "strus/fileLocatorInterface.hpp"
#include <string>
#include <vector>

namespace strus
{
/// \brief Forward declaration
struct ModuleEntryPoint;
/// \brief Forward declaration
struct AnalyzerModule;
/// \brief Forward declaration
struct StorageModule;
/// \brief Forward declaration
struct TraceModule;
/// \brief Forward declaration
class StorageClientInterface;
/// \brief Forward declaration
class QueryEvalInterface;
/// \brief Forward declaration
class DocumentAnalyzerInstanceInterface;
/// \brief Forward declaration
class QueryAnalyzerInstanceInterface;
/// \brief Forward declaration
class StorageObjectBuilderInterface;
/// \brief Forward declaration
class AnalyzerObjectBuilderInterface;
/// \brief Forward declaration
class TraceObjectBuilderInterface;
/// \brief Forward declaration
class TraceLoggerInterface;
/// \brief Forward declaration
class ErrorBufferInterface;
/// \brief Forward declaration
class DebugTraceContextInterface;
/// \brief Forward declaration
class FileLocatorInterface;


/// \brief Implementation of ModuleLoaderInterface
class ModuleLoader
	:public ModuleLoaderInterface
{
public:
	explicit ModuleLoader( ErrorBufferInterface* errorhnd_);
	virtual ~ModuleLoader();
	virtual void addSystemModulePath();
	virtual void addModulePath( const std::string& path);
	virtual bool loadModule( const std::string& name);
	virtual std::vector<std::string> moduleLoadTryPaths( const std::string& name);
	virtual void addResourcePath( const std::string& path);
	virtual void defineWorkingDirectory( const std::string& path);

	virtual std::vector<std::string> modulePaths() const		{return m_modulePaths;}
	virtual std::vector<std::string> modules() const		{return m_modules;}
	virtual std::vector<std::string> resourcePaths() const		{return m_filelocator->getResourcePaths();}
	virtual std::string workingDirectory() const			{return m_filelocator->getWorkingDirectory();}

	virtual StorageObjectBuilderInterface* createStorageObjectBuilder() const;
	virtual AnalyzerObjectBuilderInterface* createAnalyzerObjectBuilder() const;
	virtual TraceObjectBuilderInterface* createTraceObjectBuilder( const std::string& config) const;

	virtual std::vector<std::string> get3rdPartyLicenseTexts() const;
	virtual std::vector<std::string> get3rdPartyVersionTexts() const;

private:
	const ModuleEntryPoint* searchAndLoadEntryPoint( const std::string& name, std::vector<std::string>& paths_tried);
	const ModuleEntryPoint* loadModuleAlt(
			const std::string& name,
			const std::vector<std::string>& paths,
			std::vector<std::string>& paths_tried);
	const ModuleEntryPoint* tryLoadPathAsModule( const std::string& modpath);

	TraceLoggerInterface* createTraceLogger( const std::string& loggerName, const std::string& config) const;

private:
	std::vector<std::string> m_modulePaths;
	std::vector<std::string> m_modules;
	std::vector<const AnalyzerModule*> m_analyzerModules;
	std::vector<const StorageModule*> m_storageModules;
	std::vector<const TraceModule*> m_traceModules;
	std::vector<std::string> m_version_3rdparty_ar;
	std::vector<std::string> m_license_3rdparty_ar;
	std::vector<ModuleEntryPoint::Handle> m_handleList;
	ErrorBufferInterface* m_errorhnd;
	DebugTraceContextInterface* m_debugtrace;
	FileLocatorInterface* m_filelocator;
};

}//namespace
#endif

