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
class TraceProcessorInterface;
/// \brief Forward declaration
class ErrorBufferInterface;

/// \brief Structure to a processor for method call traces
struct TraceProcessorConstructor
{
	typedef TraceProcessorInterface* (*CreateTraceProcessor)( ErrorBufferInterface* errorhnd);

	const char* title;			///< title of the trace processor
	CreateTraceProcessor create;		///< constructor
};


/// \brief Structure that contains all trace module objects
struct TraceModule
	:public ModuleEntryPoint
{
	/// \brief Trace module constructor
	/// \param[in] traceProcessorConstructors_ (0,0) terminated list of method call trace processors or 0
	explicit TraceModule( const TraceProcessorConstructor* traceProcessorConstructors_);

	const TraceProcessorConstructor* traceProcessorConstructors;	///< 0 terminated list of trace processors

private:
	void init( const TraceProcessorConstructor* traceProcessorConstructors_);
};
}//namespace
#endif

