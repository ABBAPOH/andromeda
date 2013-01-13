set( HEADERS
    testplugin3.h
   )
set( SOURCES
    testplugin3.cpp
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

include_directories( ${PROJECT_SOURCE_DIR}/include )

add_library( testplugin3 SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( testplugin3 ${QT_LIBRARIES} libextensionsystem )

install_spec( testplugin3.spec )
