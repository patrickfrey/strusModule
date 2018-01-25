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
class DocumentAnalyzerInterface;
/// \brief Forward declaration
class QueryAnalyzerInterface;
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
	std::vector<std::string> m_resourcePaths;
	std::vector<const AnalyzerModule*> m_analyzerModules;
	std::vector<const StorageModule*> m_storageModules;
	std::vector<const TraceModule*> m_traceModules;
	std::vector<std::string> m_version_3rdparty_ar;
	std::vector<std::string> m_license_3rdparty_ar;
	std::vector<ModuleEntryPoint::Handle> m_handleList;
	ErrorBufferInterface* m_errorhnd;
};

}//namespace
#endif

