set( HEADERS
    testplugin2.h
   )
set( SOURCES
    testplugin2.cpp
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

include_directories( ${PROJECT_SOURCE_DIR}/include )

add_library( testplugin2 SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( testplugin2 ${QT_LIBRARIES} libextensionsystem )

install_spec( testplugin2.spec )
