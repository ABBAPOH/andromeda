if( APPLE )
    set( LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/Andromeda.app/Contents/MacOS/plugins )
else( APPLE )
    set( LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins )
endif( APPLE )

function( install_spec SPECFILE )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${SPECFILE}
  ${LIBRARY_OUTPUT_PATH}/${SPECFILE} COPYONLY)
endfunction( install_spec )

add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_NO_DEBUG )
add_definitions( -DQT_SHARED )

add_subdirectory( coreplugin )
#add_subdirectory( coreplugin/editormanager )
add_subdirectory( mainwindowplugin )
add_subdirectory( webviewplugin )
add_subdirectory( filemanagerplugin )
