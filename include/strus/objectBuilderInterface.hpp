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
#ifndef _STRUS_OBJECT_BUILDER_INTERFACE_HPP_INCLUDED
#define _STRUS_OBJECT_BUILDER_INTERFACE_HPP_INCLUDED
#include <string>

namespace strus
{

/// \brief Forward declaration
class StorageClientInterface;
/// \brief Forward declaration
class QueryEvalInterface;
/// \brief Forward declaration
class DocumentAnalyzerInterface;
/// \brief Forward declaration
class QueryAnalyzerInterface;


/// \brief Interface providing a mechanism to create complex objects used in strus
class ObjectBuilderInterface
{
public:
	/// \brief Creates a storage client and the underlying database out of a configuration string
	/// \param[in] config configuration string (not a filename!) of both storage and database
	/// \return the storage client object (with ownership returned)
	virtual StorageClientInterface* createStorageClient( const std::string& config) const=0;

	/// \brief Creates a an interface for query evaluation
	/// \return the query evaluation object (with ownership returned)
	virtual QueryEvalInterface* createQueryEval() const=0;

	/// \brief Creates a document analyzer object
	/// \param[in] segmenterName name of the segmenter used (if not specified, find the first one loaded or the default one)
	/// \return the document analyzer (with ownership returned)
	virtual DocumentAnalyzerInterface* createDocumentAnalyzer( const std::string& segmenterName=std::string()) const=0;

	/// \brief Creates a query analyzer object
	/// \return the query analyzer (with ownership returned)
	virtual QueryAnalyzerInterface* createQueryAnalyzer() const=0;
};

}//namespace
#endif
