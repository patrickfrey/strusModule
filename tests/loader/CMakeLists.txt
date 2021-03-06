cmake_minimum_required(VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
include_directories(  
  "${MODULE_INCLUDE_DIRS}" 
  "${CMAKE_CURRENT_BINARY_DIR}"
  "${strus_INCLUDE_DIRS}"
  "${strusbase_INCLUDE_DIRS}"
  "${Intl_INCLUDE_DIRS}"
)

link_directories(
   "${MAIN_SOURCE_DIR}"
   "${strusanalyzer_LIBRARY_DIRS}"
   "${strus_LIBRARY_DIRS}"
   "${strusbase_LIBRARY_DIRS}"
)

if(APPLE)
   if( ${CMAKE_GENERATOR} STREQUAL "Xcode" )
      set( STRUS_TEST_MODULE_DIRECTORY "${PROJECT_BINARY_DIR}/tests/modules/${CMAKE_BUILD_TYPE}" )
   else()
      set( STRUS_TEST_MODULE_DIRECTORY "${PROJECT_BINARY_DIR}/tests/modules" )
   endif()
else(APPLE)
   set( STRUS_TEST_MODULE_DIRECTORY  "${PROJECT_BINARY_DIR}/tests/modules" )
endif(APPLE)
configure_file( "${PROJECT_SOURCE_DIR}/tests/loader/testModuleDirectory.hpp.in"  "${PROJECT_BINARY_DIR}/tests/loader/testModuleDirectory.hpp"  @ONLY )


# -------------------------------------------
# LIBRARY
# -------------------------------------------
add_executable( testModuleLoader testModuleLoader.cpp )
target_link_libraries( testModuleLoader ${strusanalyzer_LIBRARIES} ${strus_LIBRARIES} strus_module strus_error )

add_test( LoadNormalizerModule testModuleLoader normalizer_snowball )
