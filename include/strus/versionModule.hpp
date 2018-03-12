/*
 * Copyright (c) 2014 Patrick P. Frey
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
/// \brief Header with the version info of the strus module loader
/// \file versionModule.hpp
#ifndef _STRUS_MODULE_VERSION_HPP_INCLUDED
#define _STRUS_MODULE_VERSION_HPP_INCLUDED

/// \brief Version number of the module loader
#define STRUS_MODULE_VERSION (\
	0 * 1000000\
	+ 15 * 10000\
	+ 6\
)
/// \brief Major version number of the module loader
#define STRUS_MODULE_VERSION_MAJOR 0
/// \brief Minor version number of the module loader
#define STRUS_MODULE_VERSION_MINOR 15

/// \brief The version of the module loader as string
#define STRUS_MODULE_VERSION_STRING "0.15.6"
/// \brief The signature of the module loader including the major version number
#define STRUS_MODULE_SIGNATURE "strus0"

#endif

