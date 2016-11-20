cmake_minimum_required(VERSION 2.6 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
include_directories(  
  "${PROJECT_SOURCE_DIR}/include" 
  "${strus_INCLUDE_DIRS}"
  "${strusanalyzer_INCLUDE_DIRS}"
)

link_directories(
   "${PROJECT_SOURCE_DIR}/src"
   "${strusanalyzer_LIBRARY_DIRS}"
   "${strus_LIBRARY_DIRS}"
   "${Boost_LIBRARY_DIRS}"
)


# -------------------------------------------
# TEST MODULES
# -------------------------------------------
add_library( modstrus_normalizer_snowball  MODULE  modstrus_normalizer_snowball.cpp)
set_target_properties( modstrus_normalizer_snowball PROPERTIES PREFIX "")
target_link_libraries( modstrus_normalizer_snowball strus_module strus_normalizer_snowball strus_stemmer )


