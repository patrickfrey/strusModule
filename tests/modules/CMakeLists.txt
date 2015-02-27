cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

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


# -------------------------------------------
# TEST MODULES
# -------------------------------------------
add_library( mod_normalizer_snowball  MODULE  mod_normalizer_snowball.cpp)
set_target_properties( mod_normalizer_snowball PROPERTIES PREFIX "")
target_link_libraries( mod_normalizer_snowball strus_module strus_normalizer_snowball stemmer )


