set( HEADERS
    testplugin.h
   )
set( SOURCES
    testplugin.cpp
   )

set( MOC_SOURCES "" )
qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

set( LIBRARY_OUTPUT_PATH ../plugins )

add_definitions( -DQT_PLUGIN )
add_definitions( -DQT_SHARED )

add_library( testplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )

target_link_libraries( testplugin ${QT_LIBRARIES} extensionsystem )
