set( LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins )

function( install_spec SPECFILE )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${SPECFILE}
  ${LIBRARY_OUTPUT_PATH}/${SPECFILE} COPYONLY)
endfunction( install_spec )

add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_NO_DEBUG )
add_definitions( -DQT_SHARED )

add_subdirectory( testplugin )
add_subdirectory( testplugin2 )
add_subdirectory( testplugin3 )
