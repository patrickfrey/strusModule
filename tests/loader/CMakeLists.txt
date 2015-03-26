cmake_minimum_required(VERSION 2.6 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
include_directories(  
  "${PROJECT_SOURCE_DIR}/include" 
)

link_directories(
   "${PROJECT_SOURCE_DIR}/src"
   "${strusanalyzer_LIBRARY_DIRS}"
   "${strus_LIBRARY_DIRS}"
)

set( STRUS_TEST_MODULE_DIRECTORY  "${PROJECT_SOURCE_DIR}/tests/modules" )
configure_file( "testModuleDirectory.hpp.in"  "testModuleDirectory.hpp"  @ONLY )


# -------------------------------------------
# LIBRARY
# -------------------------------------------
add_executable( testModuleLoader testModuleLoader.cpp )
target_link_libraries( testModuleLoader ${strusanalyzer_LIBRARIES} ${strus_LIBRARIES} strus_module )


