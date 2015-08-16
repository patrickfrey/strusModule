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
#include "strus/moduleLoaderInterface.hpp"
#include <string>
#include <vector>

namespace strus
{
/// \brief Forward declaration
class ModuleEntryPoint;
/// \brief Forward declaration
class AnalyzerModule;
/// \brief Forward declaration
class StorageModule;
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

/// \brief Implementation of ModuleLoaderInterface
class ModuleLoader
	:public ModuleLoaderInterface
{
public:
	ModuleLoader()
		:m_peermsgproc_enabled(false){}
	virtual ~ModuleLoader(){}
	virtual void addSystemModulePath();
	virtual void addModulePath( const std::string& path);
	virtual void loadModule( const std::string& name);
	virtual void addResourcePath( const std::string& path);
	virtual void enablePeerMessageProcessor( const std::string& name);

	virtual StorageObjectBuilderInterface* createStorageObjectBuilder() const;
	virtual AnalyzerObjectBuilderInterface* createAnalyzerObjectBuilder() const;

private:
	const ModuleEntryPoint* loadModuleAlt(
			const std::string& name,
			const std::vector<std::string>& paths);

private:
	std::vector<std::string> m_modulePaths;
	std::vector<std::string> m_resourcePaths;
	std::vector<const AnalyzerModule*> m_analyzerModules;
	std::vector<const StorageModule*> m_storageModules;
	std::string m_peermsgproc;
	bool m_peermsgproc_enabled;
};

}//namespace
#endif

