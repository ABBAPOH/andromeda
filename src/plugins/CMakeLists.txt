set( LIBRARY_OUTPUT_PATH ${APP_PLUGIN_PATH} )

include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${PROJECT_SOURCE_DIR}/src/plugins)

add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_SHARED )
if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    add_definitions( -DQT_NO_DEBUG )
endif()

add_subdirectory( bineditorplugin )
add_subdirectory( bookmarksplugin )
add_subdirectory( filemanagerplugin )
add_subdirectory( helloworldplugin )
add_subdirectory( imageviewerplugin )
add_subdirectory( texteditorplugin )
add_subdirectory( pdfviewplugin )
add_subdirectory( webviewplugin )
