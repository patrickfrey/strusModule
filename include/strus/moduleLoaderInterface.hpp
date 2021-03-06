/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Interface of the strus module loader
/// \file moduleLoaderInterface.hpp
#ifndef _STRUS_MODULE_LOADER_INTERFACE_HPP_INCLUDED
#define _STRUS_MODULE_LOADER_INTERFACE_HPP_INCLUDED
#include <string>
#include <vector>

/// \brief strus toplevel namespace
namespace strus
{
/// \brief Forward declaration
class StorageObjectBuilderInterface;
/// \brief Forward declaration
class AnalyzerObjectBuilderInterface;
/// \brief Forward declaration
class TraceObjectBuilderInterface;

/// \brief Interface providing a mechanism to load modules and to create the objects defined in the modules
class ModuleLoaderInterface
{
public:
	/// \brief Destructor
	virtual ~ModuleLoaderInterface(){}

	/// \brief Add the path defined by the system depending on the platform where to seek modules to load
	/// \note If you do not define any path with 'addSystemModulePath()' or 'addModulePath(const std::string&)' then the system module path is used for loading modules.
	virtual void addSystemModulePath()=0;

	/// \brief Add a new path where to seek modules to load
	/// \param[in] path full path of the modules
	virtual void addModulePath( const std::string& path)=0;

	/// \brief Load a module with name 'name' seeking in all module paths defined in the order of their definition
	/// \param[in] name name of the module with or without file extension (default file extension depends on platform)
	virtual bool loadModule( const std::string& name)=0;

	/// \brief Get the list of files tried to load for module with a given name
	/// \param[in] name name of the module with or without file extension (default file extension depends on platform)
	virtual std::vector<std::string> moduleLoadTryPaths( const std::string& name)=0;

	/// \brief Declare a path for analyzer components to find resource files
	/// \param[in] path path to add
	virtual void addResourcePath( const std::string& path)=0;

	/// \brief Declare the root path for all data created and written by strus modules
	/// \param[in] path path to define as root path
	virtual void defineWorkingDirectory( const std::string& path)=0;

	/// \brief Get the paths where to seek modules to load
	/// \return list of paths in order of their definition
	virtual std::vector<std::string> modulePaths() const=0;

	/// \brief Get the names of loaded modules
	/// \return the names of loaded modules in the order of their definition
	virtual std::vector<std::string> modules() const=0;

	/// \brief Get the paths for resource files
	/// \return the paths for resource files
	virtual std::vector<std::string> resourcePaths() const=0;

	/// \brief Get the root path for all data created and written by strus modules
	/// \return data root path
	virtual std::string workingDirectory() const=0;

	/// \brief Create a builder for storage objects build from components loaded from module or the defaults defined.
	/// \return the builder object (with ownership)
	virtual StorageObjectBuilderInterface* createStorageObjectBuilder() const=0;

	/// \brief Create a builder for analyzer objects build from components loaded from module or the defaults defined.
	/// \return the builder object (with ownership)
	virtual AnalyzerObjectBuilderInterface* createAnalyzerObjectBuilder() const=0;

	/// \brief Get the builder for call trace proxy objects for analyzer or storage.
	///		The returned builder is built from components loaded from module or the standard builders defined by name.
	/// \param[in] config trace object builder config
	/// \return the builder object (with ownership)
	virtual TraceObjectBuilderInterface* createTraceObjectBuilder( const std::string& config) const=0;

	/// \brief Get the license texts of loaded 3rdParty components, that need to be visible
	/// \return the list of license tests
	virtual std::vector<std::string> get3rdPartyLicenseTexts() const=0;

	/// \brief Get the version info of loaded 3rdParty components, if of any interest
	/// \return the list of version strings
	virtual std::vector<std::string> get3rdPartyVersionTexts() const=0;
};

}//namespace
#endif


