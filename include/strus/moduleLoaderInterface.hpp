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
#ifndef _STRUS_MODULE_LOADER_HPP_INCLUDED
#define _STRUS_MODULE_LOADER_HPP_INCLUDED
#include <string>

namespace strus
{

/// \brief Forward declaration
class AnalyzerInterface;
/// \brief Forward declaration
class StorageInterface;
/// \brief Forward declaration
class QueryProcessorInterface;
/// \brief Forward declaration
class QueryEvalInterface;
/// \brief Forward declaration
class QueryAnalyzerInterface;
/// \brief Forward declaration
class DocumentAnalyzerInterface;
/// \brief Forward declaration
class TextProcessorInterface;
/// \brief Forward declaration
class SegmenterInterface;


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
	virtual void loadModule( const std::string& name)=0;

	virtual QueryAnalyzerConstructor queryAnalyzerConstructor( const std::string& name);
	virtual DocumentAnalyzerConstructor documentAnalyzerConstructor( const std::string& name);
	
};

}//namespace
#endif

