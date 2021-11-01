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
	${CMAKE_CURRENT_BINARY_DIR}
	${MAIN_SOURCE_DIR}
)

link_directories(
	"${strusbase_LIBRARY_DIRS}"
	"${strus_LIBRARY_DIRS}"
	"${strusanalyzer_LIBRARY_DIRS}"
	"${strustrace_LIBRARY_DIRS}"
	"${Boost_LIBRARY_DIRS}"
)


# -------------------------------------------
# LIBRARY
# -------------------------------------------
add_cppcheck( strus_module ${source_files_loader} ${source_files_module} analyzerModule.cpp storageModule.cpp traceModule.cpp strusModuleInfo.cpp internationalization.cpp )

add_library( strus_module SHARED ${source_files_loader} )
target_link_libraries( strus_module ${CMAKE_DL_LIBS} strus_base strus_traceproc_std strus_traceobj strus_analyzer strus_segmenter_textwolf strus_segmenter_cjson strus_segmenter_tsv strus_textproc strus_queryeval strus_queryproc strus_statsproc strus_storage strus_statsstorage strus_database_leveldb strus_detector_std strus_contentstats_std strus_filelocator ${Intl_LIBRARIES} )

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

add_library( modstrus_analyzer_pattern_test  MODULE  modstrus_analyzer_pattern_test.cpp )
set_target_properties( modstrus_analyzer_pattern_test PROPERTIES PREFIX "")
target_link_libraries( modstrus_analyzer_pattern_test strus_module_analyzer strus_pattern_test ${Intl_LIBRARIES} )

# Copy pattern module into a created directory easy relocatable by tests:
add_custom_command( TARGET modstrus_analyzer_pattern_test POST_BUILD
					  COMMAND ${CMAKE_COMMAND} -E make_directory  ${CMAKE_BINARY_DIR}/modules/strus )
add_custom_command( TARGET modstrus_analyzer_pattern_test POST_BUILD
					  COMMAND ${CMAKE_COMMAND} -E copy_if_different  $<TARGET_FILE:modstrus_analyzer_pattern_test>  ${CMAKE_BINARY_DIR}/modules/strus/
					  COMMENT "Copy strus test pattern module built to ${CMAKE_BINARY_DIR}/modules/strus/" )


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

install( TARGETS modstrus_analyzer_pattern_test
           LIBRARY DESTINATION ${LIB_INSTALL_DIR}/strus/modules
	   RUNTIME DESTINATION bin )


