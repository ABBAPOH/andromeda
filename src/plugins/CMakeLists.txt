set( LIBRARY_OUTPUT_PATH ${APP_PLUGIN_PATH} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/guisystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )
include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin )

add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_SHARED )
if( CMAKE_BUILD_TYPE STREQUAL "Release" )
    add_definitions( -DQT_NO_DEBUG )
endif()

add_subdirectory( coreplugin )
add_subdirectory( filemanagerplugin )
add_subdirectory( helloworldplugin )
add_subdirectory( webviewplugin )
