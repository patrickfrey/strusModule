cmake_minimum_required(VERSION 2.6 FATAL_ERROR )

# must be done BEFORE project in order to work!
include(cmake/install_prefix.cmake)

# Project globals:
project( StrusModule )
set( STRUS_MAJOR_VERSION 0 )
set( STRUS_MINOR_VERSION 1 )
set( STRUS_PATCH_VERSION 5 )
set( STRUS_VERSION ${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}.${STRUS_PATCH_VERSION} )

include( cmake/dist.cmake )
include( cmake/LinuxDistribution.cmake )
include( cmake/build_rules.cmake )
include( cmake/find_strus_package.cmake )

find_package( Boost 1.53.0 COMPONENTS thread system date_time )
find_strus_package( core )
find_strus_package( analyzer )

IF (WIN32)
set( STRUS_MODULE_EXTENSION  ".dll" )
set( STRUS_MODULE_PATHSEP ';')
set( STRUS_MODULE_DIRECTORIES  "${strus_LIBRARY_DIRS}/modules;${strusanalyzer_LIBRARY_DIRS}/modules" )
ELSE (WIN32)
set( STRUS_MODULE_EXTENSION  ".so" )
set( STRUS_MODULE_PATHSEP ':')
set( STRUS_MODULE_DIRECTORIES  "${strus_LIBRARY_DIRS}/modules:${strusanalyzer_LIBRARY_DIRS}/modules" )
ENDIF (WIN32)

set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
configure_file( "${PROJECT_SOURCE_DIR}/include/strus/versionModule.hpp.in"  "${PROJECT_SOURCE_DIR}/include/strus/versionModule.hpp"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/src/moduleDirectory.hpp.in"  "${PROJECT_SOURCE_DIR}/src/moduleDirectory.hpp"  @ONLY )

set( STRUS_GETTEXT_PACKAGE "strus-dom" )       #... 1st parameter of bindtextdomain(...)
set( STRUS_GETTEXT_LOCALEDIR "" )                   #... 2nd parameter of bindtextdomain(...)
configure_file( "${PROJECT_SOURCE_DIR}/src/internationalization.cpp.in"  "${PROJECT_SOURCE_DIR}/src/internationalization.cpp"  @ONLY )

enable_testing()

# Subdirectories:
add_subdirectory( src )
add_subdirectory( tests )
  
include( cmake/report_build_settings.cmake )

# Install header files:
install(DIRECTORY include/strus
          DESTINATION include
          FILES_MATCHING PATTERN "*.hpp")

include( cmake/standard_targets.cmake )

