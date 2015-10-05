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
/// \brief Interface of the strus module loader
/// \file moduleLoaderInterface.hpp
#ifndef _STRUS_MODULE_LOADER_INTERFACE_HPP_INCLUDED
#define _STRUS_MODULE_LOADER_INTERFACE_HPP_INCLUDED
#include <string>

/// \brief strus toplevel namespace
namespace strus
{
/// \brief Forward declaration
class StorageObjectBuilderInterface;
/// \brief Forward declaration
class AnalyzerObjectBuilderInterface;

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

	/// \brief Declare a path for analyzer components to find resource files
	/// \param[in] path path to add
	virtual void addResourcePath( const std::string& path)=0;

	/// \brief Enable and declare the processor for packing/unpacking messages sent to peers
	/// \param[in] name of the processor
	virtual void enablePeerMessageProcessor( const std::string& name)=0;

	/// \brief Get the builder for storage objects build from components loaded from module or the defaults defined.
	/// \return the builder object
	virtual StorageObjectBuilderInterface* createStorageObjectBuilder() const=0;

	/// \brief Get the builder for analyzer objects build from components loaded from module or the defaults defined.
	/// \return the builder object
	virtual AnalyzerObjectBuilderInterface* createAnalyzerObjectBuilder() const=0;
};

}//namespace
#endif


