if( APPLE )
    set( LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/Andromeda.app/Contents/plugins/andromeda )
else( APPLE )
    set( LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins/andromeda )
endif( APPLE )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/guisystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )
include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin )

add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_NO_DEBUG )
add_definitions( -DQT_SHARED )

add_subdirectory( coreplugin )
add_subdirectory( webviewplugin )
add_subdirectory( filemanagerplugin )
