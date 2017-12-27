cmake_minimum_required(VERSION 2.8 FATAL_ERROR )

# --------------------------------------
# SOURCES AND INCLUDES
# --------------------------------------
set( source_files_loader
	libstrus_module.cpp
	moduleEntryPoint.cpp
	${CMAKE_CURRENT_BINARY_DIR}/internationalization.cpp
	analyzerModule.cpp
	storageModule.cpp
	traceModule.cpp
	storageObjectBuilder.cpp
	analyzerObjectBuilder.cpp
	moduleLoader.cpp
)

set( source_files_module
	moduleEntryPoint.cpp
	${CMAKE_CURRENT_BINARY_DIR}/internationalization.cpp
)

include_directories(
	"${MODULE_INCLUDE_DIRS}"
	"${strusbase_INCLUDE_DIRS}"
	"${strus_INCLUDE_DIRS}"
	"${strusanalyzer_INCLUDE_DIRS}"
	"${strustrace_INCLUDE_DIRS}"
	"${Boost_INCLUDE_DIRS}"
	"${Intl_INCLUDE_DIRS}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	"${MAIN_SOURCE_DIR}"
)

link_directories(
	"${strusbase_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
	"${strusanalyzer_LIBRARY_DIRS}"
	"${strustrace_LIBRARY_DIRS}"
	"${Boost_LIBRARY_DIRS}"
	${Intl_LIBRARIES}
)


# -------------------------------------------
# LIBRARY
# -------------------------------------------
add_library( strus_module SHARED ${source_files_loader} )
target_link_libraries( strus_module ${CMAKE_DL_LIBS} strus_base strus_traceproc_std strus_traceobj strus_analyzer strus_segmenter_textwolf strus_segmenter_cjson strus_segmenter_tsv strus_textproc strus_queryeval strus_queryproc strus_statsproc strus_storage strus_database_leveldb ${Intl_LIBRARIES} )

add_library( strus_module_analyzer SHARED ${source_files_module} analyzerModule.cpp )
target_link_libraries( strus_module_analyzer ${CMAKE_DL_LIBS} strus_base ${Intl_LIBRARIES} )

add_library( strus_module_storage SHARED ${source_files_module} storageModule.cpp )
target_link_libraries( strus_module_storage ${CMAKE_DL_LIBS} strus_base ${Intl_LIBRARIES} )

add_library( strus_module_trace SHARED ${source_files_module} traceModule.cpp )
target_link_libraries( strus_module_trace ${CMAKE_DL_LIBS} strus_base ${Intl_LIBRARIES} )

add_executable( strusModuleInfo strusModuleInfo.cpp internationalization.cpp )
target_link_libraries( strusModuleInfo  "${Boost_LIBRARIES}" strus_module strus_module_analyzer  strus_module_storage strus_error strus_base ${Intl_LIBRARIES})

set_target_properties(
    strus_module
    PROPERTIES
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION ${STRUS_VERSION}
)

set_target_properties(
    strus_module_analyzer
    PROPERTIES
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION ${STRUS_VERSION}
)

set_target_properties(
    strus_module_storage
    PROPERTIES
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION ${STRUS_VERSION}
)

set_target_properties(
    strus_module_trace
    PROPERTIES
    DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
    SOVERSION "${STRUS_MAJOR_VERSION}.${STRUS_MINOR_VERSION}"
    VERSION ${STRUS_VERSION}
)

# ------------------------------
# INSTALLATION
# ------------------------------
install( TARGETS strus_module
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus )
install( TARGETS strus_module_analyzer
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus )
install( TARGETS strus_module_storage
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus )
install( TARGETS strus_module_trace
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus )
	   
install( TARGETS strusModuleInfo 
	   RUNTIME DESTINATION bin )

