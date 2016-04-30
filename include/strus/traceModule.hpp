/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Method call trace module object type declarations
/// \file traceModule.hpp
#ifndef _STRUS_MODULE_TRACE_HPP_INCLUDED
#define _STRUS_MODULE_TRACE_HPP_INCLUDED
#include "strus/moduleEntryPoint.hpp"
#include <string>

/// \brief strus toplevel namespace
namespace strus
{

/// \brief Forward declaration
class TraceLoggerInterface;
/// \brief Forward declaration
class ErrorBufferInterface;

/// \brief Structure describing a logger for method call traces
struct TraceLoggerConstructor
{
	/// \param[in] config configuration string of the logger (semicolon (Unix) or colon (Windows) separated list of attribute=value assignments)
	/// \param[in] errorhnd error buffer interface
	typedef TraceLoggerInterface* (*CreateTraceLogger)( const std::string& config, ErrorBufferInterface* errorhnd);

	const char* title;			///< title of the trace logger
	CreateTraceLogger create;		///< constructor
};


/// \brief Structure that contains all trace module objects
struct TraceModule
	:public ModuleEntryPoint
{
	/// \brief Trace module constructor
	/// \param[in] traceLoggerConstructors_ (0,0) terminated list of method call trace Loggers or 0
	explicit TraceModule( const TraceLoggerConstructor* traceLoggerConstructors_);

	const TraceLoggerConstructor* traceLoggerConstructors;	///< 0 terminated list of trace Loggers

private:
	void init( const TraceLoggerConstructor* traceLoggerConstructors_);
};
}//namespace
#endif

