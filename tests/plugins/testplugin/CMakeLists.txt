set( HEADERS
    testplugin.h
   )
set( SOURCES
    testplugin.cpp
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )

add_library( testplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( testplugin ${QT_LIBRARIES} extensionsystem )

install_spec( testplugin.spec )
