project( andromeda )

cmake_minimum_required( VERSION 2.6 )

# common Qt directives
find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
add_definitions( -DUNICODE )

include( "cmake/Andromeda.cmake" )
include( "cmake/Install.cmake" )
include( "cmake/ProjectInfo.cmake" )

include_directories( ${QT_INCLUDES} )

add_subdirectory( src )
add_subdirectory( tests )

# make uninstall
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/Uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/Uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/Uninstall.cmake)
