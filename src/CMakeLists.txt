cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	moduleEntryPoint.cpp
	moduleAnalyzer.cpp
	moduleStorage.cpp
)

include_directories( "${PROJECT_SOURCE_DIR}/include"  "${strus_INCLUDE_DIRS}"  "${strusanalyzer_INCLUDE_DIRS}" )


# -------------------------------------------
# LIBRARY
# -------------------------------------------
add_library( strus_module SHARED ${source_files})
target_link_libraries( strus_module )
set_target_properties(
    strus_module
    PROPERTIES
    DEBUG_POSTFIX "_d"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION ${STRUS_VERSION}
)


# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_module
           LIBRARY DESTINATION lib/strus )

