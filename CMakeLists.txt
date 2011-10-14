project( andromeda )

cmake_minimum_required( VERSION 2.6 )

find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
add_definitions( -DUNICODE )

include( "LibSuffix.cmake" )

if(APPLE)
    set( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
    set( QM_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/Andromeda.app/Contents/Resources/translations )
elseif(WIN32)
    set( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
    set( QM_OUTPUT_PATH ${PROJECT_BINARY_DIR}/translations )
elseif(UNIX)
    set( LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib${LIB_SUFFIX} )
    set( QM_OUTPUT_PATH ${PROJECT_BINARY_DIR}/share/andromeda/translations )
endif()

include( "Install.cmake" )
include( "ProjectInfo.cmake" )
include( "Linguist.cmake" )

add_definitions( -DGIT_REVISION=\"${GIT_REVISION}\" )
add_definitions( -DPROJECT_VERSION=\"${PROJECT_VERSION}\" )

set( EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin )
#-DCMAKE_BUILD_TYPE=Debug

if(MSVC)
    set(CMAKE_SHARED_LIBRARY_PREFIX "lib")
else(MSVC)
    add_definitions( -Wall )
endif(MSVC)

include_directories( ${QT_INCLUDES} )

add_subdirectory( src )

if( ENABLE_TESTS )
    enable_testing()

    add_subdirectory( tests )

    set( CTEST_ARGUMENTS --output-on-failure )
    add_custom_target( test-verbose )
    add_custom_command( TARGET test-verbose POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND} ${CTEST_ARGUMENTS} )
endif()

# make uninstall
configure_file(
    "${CMAKE_SOURCE_DIR}/Uninstall.cmake.in"
    "${CMAKE_BINARY_DIR}/Uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/Uninstall.cmake)
