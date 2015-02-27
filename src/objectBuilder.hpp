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
#ifndef _STRUS_MODULE_OBJECT_BUILDER_HPP_INCLUDED
#define _STRUS_MODULE_OBJECT_BUILDER_HPP_INCLUDED
#include "strus/objectBuilderInterface.hpp"
#include "strus/reference.hpp"
#include "strus/queryProcessorInterface.hpp"
#include "strus/textProcessorInterface.hpp"
#include <string>
#include <vector>

namespace strus
{
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
class DatabaseInterface;
/// \brief Forward declaration
class SegmenterInterface;


/// \brief Implementation of ObjectBuilderInterface for the module loader
class ObjectBuilder
	:public ObjectBuilderInterface
{
public:
	ObjectBuilder();
	virtual ~ObjectBuilder(){}

	virtual StorageClientInterface* createStorageClient( const std::string& config) const;
	virtual QueryEvalInterface* createQueryEval() const;
	virtual DocumentAnalyzerInterface* createDocumentAnalyzer( const std::string& segmenterName) const;
	virtual QueryAnalyzerInterface* createQueryAnalyzer() const;

public/*ModuleLoader*/:
	void addStorageModule( const StorageModule* mod);
	void addAnalyzerModule( const AnalyzerModule* mod);

private:
	const DatabaseInterface* getDatabase( const std::string& name) const;
	SegmenterInterface* createSegmenter( const std::string& name) const;

private:
	std::vector<const AnalyzerModule*> m_analyzerModules;
	std::vector<const StorageModule*> m_storageModules;
	Reference<QueryProcessorInterface> m_queryProcessor;
	Reference<TextProcessorInterface> m_textProcessor;
};

}//namespace
#endif
