set( HEADERS
    testplugin.h
   )
set( SOURCES
    testplugin.cpp
   )

set( HEADERS2
    testplugin2.h
   )
set( SOURCES2
    testplugin2.cpp
   )

set( HEADERS3
    testplugin3.h
   )
set( SOURCES3
    testplugin3.cpp
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_cpp( MOC_SOURCES2 ${HEADERS2} )
qt4_wrap_cpp( MOC_SOURCES3 ${HEADERS3} )

set( LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH}/plugins )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../ )
link_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../ )

add_definitions( ${QT_DEFINITIONS} )
add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_NO_DEBUG )
add_definitions( -DQT_SHARED )

add_library( testplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( testplugin ${QT_LIBRARIES} extensionsystem )

add_library( testplugin2 SHARED ${HEADERS2} ${SOURCES2} ${MOC_SOURCES2} )
target_link_libraries( testplugin2 ${QT_LIBRARIES} extensionsystem )

add_library( testplugin3 SHARED  ${HEADERS3} ${SOURCES3} ${MOC_SOURCES3} )
target_link_libraries( testplugin3 ${QT_LIBRARIES} extensionsystem )


#if (WIN32)
    # WIN32 is specified on Windows to remove console
#    add_executable( test WIN32 main.cpp )
#else (WIN32)
    add_executable( extensionsystem_test main.cpp )
#endif (WIN32)
target_link_libraries( extensionsystem_test ${QT_LIBRARIES} extensionsystem )
