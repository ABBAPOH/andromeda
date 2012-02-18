set( LIBRARY_OUTPUT_PATH ${APP_LIBRARY_PATH} )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ )

add_subdirectory( extensionsystem )
add_subdirectory( guisystem )
add_subdirectory( io )
add_subdirectory( widgets )
