function( install_spec SPECFILE )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${SPECFILE} ${LIBRARY_OUTPUT_PATH}/${SPECFILE} COPYONLY)
#    install(CODE "execute_process(COMMAND mkdir -p ${LIBRARY_OUTPUT_PATH})" )
#    install(CODE "execute_process(COMMAND ${CMAKE_BINARY_DIR}/bin/plugineditor ${CMAKE_CURRENT_SOURCE_DIR}/${SPECFILE} ${LIBRARY_OUTPUT_PATH}/${SPECFILE} )" )
endfunction( install_spec )

if(UNIX AND NOT APPLE)
    set( INSTALL_BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin )
    set( INSTALL_LIB_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/andromeda )
    set( INSTALL_PLUGIN_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}/andromeda/plugins/andromeda )

    set( CMAKE_INSTALL_RPATH_USE_LINK_PATH true )
    set( CMAKE_INSTALL_RPATH ${INSTALL_LIB_DIR} )

    function( install_lib TARGET )
        install( TARGETS ${TARGET} DESTINATION ${INSTALL_LIB_DIR} )
    endfunction( install_lib )

    function( install_bin TARGET )
        install( TARGETS ${TARGET} DESTINATION ${INSTALL_BIN_DIR} )
    endfunction( install_bin )

    function( install_plugin TARGET )
        install( TARGETS ${TARGET} DESTINATION ${INSTALL_PLUGIN_DIR} )
        install( FILES ${TARGET}.spec DESTINATION ${INSTALL_PLUGIN_DIR} )
	install( CODE "execute_process(COMMAND ${CMAKE_BINARY_DIR}/bin/plugineditor ${CMAKE_CURRENT_SOURCE_DIR}/${TARGET}.spec ${INSTALL_PLUGIN_DIR}/${TARGET}.spec)" )
        install_spec(${TARGET}.spec)
    endfunction( install_plugin )
else()
    function( install_lib TARGET )
    endfunction( install_lib )

    function( install_bin TARGET )
    endfunction( install_bin )

    function( install_plugin TARGET )
        install_spec(${TARGET}.spec)
    endfunction( install_plugin )
endif()
