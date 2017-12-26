cmake_minimum_required(VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
include_directories(  
  "${MODULE_INCLUDE_DIRS}" 
  "${strusbase_INCLUDE_DIRS}"
  "${strus_INCLUDE_DIRS}"
  "${strusanalyzer_INCLUDE_DIRS}"
  "${strustrace_INCLUDE_DIRS}"
  "${CMAKE_CURRENT_BINARY_DIR}/include"
)

link_directories(
   "${MAIN_SOURCE_DIR}"
   "${strus_LIBRARY_DIRS}"
   "${strusanalyzer_LIBRARY_DIRS}"
   "${strustrace_LIBRARY_DIRS}"
   "${Boost_LIBRARY_DIRS}"
   "${Intl_INCLUDE_DIRS}"
)


# -------------------------------------------
# TEST MODULES
# -------------------------------------------
add_library( modstrus_normalizer_snowball  MODULE  modstrus_normalizer_snowball.cpp)
set_target_properties( modstrus_normalizer_snowball PROPERTIES PREFIX "")
target_link_libraries( modstrus_normalizer_snowball strus_module strus_normalizer_snowball strus_stemmer )


