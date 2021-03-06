cmake_minimum_required(VERSION 2.8 FATAL_ERROR )

# must be done BEFORE project in order to work!
include(cmake/install_prefix.cmake)

# Project globals:
project( StrusModule )
set( STRUS_MAJOR_VERSION 0 )
set( STRUS_MINOR_VERSION 17 )
set( STRUS_PATCH_VERSION 0 )
set( STRUS_VERSION ${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}.${STRUS_PATCH_VERSION} )

if(NOT STRUS_ALL)
set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
include( cmake/cxx11.cmake )
check_for_cxx11_compiler( HAVE_CXX11 )
include( cmake/dist.cmake )
include( cmake/LinuxDistribution.cmake )
include( cmake/build_rules.cmake )
include( cmake/boost.cmake )
include( cmake/find_strus_package.cmake )
include( cmake/link_rules.cmake )
include( cmake/intl.cmake )
include( cmake/libdl.cmake )
include( cmake/cppcheck.cmake )
endif()

find_strus_package( base )
find_strus_package( core )
find_strus_package( analyzer )
find_strus_package( trace )
set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
configure_file( "${PROJECT_SOURCE_DIR}/include/strus/versionModule.hpp.in"  "${CMAKE_CURRENT_BINARY_DIR}/include/strus/versionModule.hpp"  @ONLY )

IF (WIN32)
if( NOT DEFINED STRUS_MODULE_EXTENSION )
set( STRUS_MODULE_EXTENSION  ".dll" )
endif( NOT DEFINED STRUS_MODULE_EXTENSION )
set( STRUS_MODULE_PATHSEP ';')
if( NOT DEFINED STRUS_MODULE_DIRECTORIES )
set( STRUS_MODULE_DIRECTORIES  "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/strus/modules" )
endif( NOT DEFINED STRUS_MODULE_DIRECTORIES )
ELSE (WIN32)
if( NOT DEFINED STRUS_MODULE_EXTENSION )
set( STRUS_MODULE_EXTENSION  ".so" )
endif( NOT DEFINED STRUS_MODULE_EXTENSION )
set( STRUS_MODULE_PATHSEP ':')
if( NOT DEFINED STRUS_MODULE_DIRECTORIES )
set( STRUS_MODULE_DIRECTORIES  "${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/strus/modules" )
endif( NOT DEFINED STRUS_MODULE_DIRECTORIES )
ENDIF (WIN32)

set( CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )
set( CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON )
configure_file( "${PROJECT_SOURCE_DIR}/include/strus/versionModule.hpp.in"  "${CMAKE_CURRENT_BINARY_DIR}/include/strus/versionModule.hpp"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/src/moduleDirectory.hpp.in"  "${CMAKE_CURRENT_BINARY_DIR}/src/moduleDirectory.hpp"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh.in"  "${PROJECT_SOURCE_DIR}/dist/obs/deploy_to_obs.sh"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD.in"  "${PROJECT_SOURCE_DIR}/dist/archlinux/PKGBUILD"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/obs/strusmodule.dsc.in"  "${PROJECT_SOURCE_DIR}/dist/obs/strusmodule.dsc"  @ONLY )
configure_file( "${PROJECT_SOURCE_DIR}/dist/redhat/strusmodule.spec.in"  "${PROJECT_SOURCE_DIR}/dist/redhat/strusmodule.spec"  @ONLY )

set( STRUS_GETTEXT_PACKAGE "strus-dom" )       #... 1st parameter of bindtextdomain(...)
set( STRUS_GETTEXT_LOCALEDIR "" )                   #... 2nd parameter of bindtextdomain(...)
configure_file( "${PROJECT_SOURCE_DIR}/src/internationalization.cpp.in"  "${CMAKE_CURRENT_BINARY_DIR}/src/internationalization.cpp"  @ONLY )

enable_testing()

# Path declarations:
set( MODULE_INCLUDE_DIRS            "${CMAKE_CURRENT_BINARY_DIR}/include"  "${PROJECT_SOURCE_DIR}/include" )
set( MAIN_SOURCE_DIR                     "${PROJECT_SOURCE_DIR}/src" )
set( MAIN_TESTS_DIR                        "${PROJECT_SOURCE_DIR}/tests" )

# Create directory holding strus modules
file( MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/modules")
install( DIRECTORY "${CMAKE_BINARY_DIR}/modules" DESTINATION ${LIB_INSTALL_DIR}/strus/ )

# Subdirectories:
add_subdirectory( src )
add_subdirectory( tests )
  
if(NOT STRUS_ALL)
include( cmake/report_build_settings.cmake )
endif()

# Install header files:
install(DIRECTORY include/strus
          DESTINATION include
          FILES_MATCHING PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include/strus
          DESTINATION include
          FILES_MATCHING PATTERN "versionModule.hpp")

if(NOT STRUS_ALL)
include( cmake/standard_targets.cmake )
endif()
