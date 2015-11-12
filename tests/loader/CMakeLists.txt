cmake_minimum_required(VERSION 2.6 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
include_directories(  
  "${PROJECT_SOURCE_DIR}/include" 
  "${CMAKE_CURRENT_BINARY_DIR}"
)

link_directories(
   "${PROJECT_SOURCE_DIR}/src"
   "${strusanalyzer_LIBRARY_DIRS}"
   "${strus_LIBRARY_DIRS}"
)

set( STRUS_TEST_MODULE_DIRECTORY  "${PROJECT_BINARY_DIR}/tests/modules" )
configure_file( "${PROJECT_SOURCE_DIR}/tests/loader/testModuleDirectory.hpp.in"  "${PROJECT_BINARY_DIR}/tests/loader/testModuleDirectory.hpp"  @ONLY )


# -------------------------------------------
# LIBRARY
# -------------------------------------------
add_executable( testModuleLoader testModuleLoader.cpp )
target_link_libraries( testModuleLoader ${strusanalyzer_LIBRARIES} ${strus_LIBRARIES} strus_module strus_error )

add_test( LoadNormalizerModule testModuleLoader normalizer_snowball )
