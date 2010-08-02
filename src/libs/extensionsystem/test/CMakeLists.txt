set( HEADERS
    testplugin.h
   )
set( SOURCES
    testplugin.cpp
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

set( LIBRARY_OUTPUT_PATH plugins )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../ )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../ )

add_definitions( ${QT_DEFINITIONS} )
add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_NO_DEBUG )
add_definitions( -DQT_SHARED )

add_library( testplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( testplugin ${QT_LIBRARIES} extensionsystem )

#if (WIN32)
    # WIN32 is specified on Windows to remove console
#    add_executable( test WIN32 main.cpp )
#else (WIN32)
    add_executable( test main.cpp )
#endif (WIN32)
target_link_libraries( test ${QT_LIBRARIES} extensionsystem )
