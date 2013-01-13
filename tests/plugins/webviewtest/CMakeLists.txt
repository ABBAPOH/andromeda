if( ${QT_QTWEBKIT_FOUND} )

set( QT_USE_QTWEBKIT TRUE )
include( ${QT_USE_FILE} )

set( HEADERS
    webviewtest.h
   )
set( SOURCES
    webviewtest.cpp
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

include_directories( ${PROJECT_SOURCE_DIR}/include )

add_library( webviewtest SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( webviewtest ${QT_LIBRARIES} libextensionsystem )

install_spec( webviewtest.spec )

endif( ${QT_QTWEBKIT_FOUND} )
