set( LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins )

add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_NO_DEBUG )
add_definitions( -DQT_SHARED )

add_subdirectory( testplugin )
add_subdirectory( testplugin2 )
add_subdirectory( testplugin3 )
