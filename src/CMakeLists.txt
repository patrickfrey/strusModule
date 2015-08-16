cmake_minimum_required(VERSION 2.6 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files
	libstrus_module.cpp
	moduleEntryPoint.cpp
	analyzerModule.cpp
	storageModule.cpp
	storageObjectBuilder.cpp
	analyzerObjectBuilder.cpp
	moduleLoader.cpp
	utils.cpp
)

include_directories(
	"${PROJECT_SOURCE_DIR}/include"
	"${strus_INCLUDE_DIRS}"
	"${strusanalyzer_INCLUDE_DIRS}" )

link_directories(
	"${PROJECT_SOURCE_DIR}/src"
	"${strusanalyzer_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
)


# -------------------------------------------
# LIBRARY
# -------------------------------------------
add_library( strus_module SHARED ${source_files})
target_link_libraries( strus_module dl strus_analyzer strus_segmenter_textwolf  strus_textproc strus_queryeval strus_queryproc strus_peermsgproc strus_storage strus_database_leveldb )
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
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus )

