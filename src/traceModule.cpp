/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
#include "strus/traceModule.hpp"
#include "strus/base/dll_tags.hpp"
#include <string>

using namespace strus;

DLL_PUBLIC TraceModule::TraceModule(
		const TraceProcessorConstructor* traceProcessorConstructors_)
	:ModuleEntryPoint(ModuleEntryPoint::Trace)
{
	init( traceProcessorConstructors_);
}

void TraceModule::init(
		const TraceProcessorConstructor* traceProcessorConstructors_)
{
	traceProcessorConstructors = traceProcessorConstructors_;
}

