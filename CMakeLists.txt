project( Andromeda )

cmake_minimum_required( VERSION 2.6 )

find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
add_definitions( -DUNICODE )

set( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )

add_subdirectory( src )

enable_testing()

add_subdirectory( tests )

set( CTEST_ARGUMENTS --output-on-failure )
add_custom_target( test-verbose )
add_custom_command( TARGET test-verbose POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND} ${CTEST_ARGUMENTS} )

