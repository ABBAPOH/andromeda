set( LIBRARY_OUTPUT_PATH ${APP_LIBRARY_PATH} )

include_directories( ${PROJECT_SOURCE_DIR}/include )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/ )

#override prefix so we can name library targets as libName
set( CMAKE_SHARED_LIBRARY_PREFIX "" )

add_subdirectory( extensionsystem )
add_subdirectory( guisystem )
add_subdirectory( io )
add_subdirectory( widgets )
add_subdirectory( imageviewer )
