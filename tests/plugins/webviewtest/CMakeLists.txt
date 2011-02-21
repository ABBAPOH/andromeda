set( QT_USE_QTWEBKIT TRUE )
include( ${QT_USE_FILE} )

set( HEADERS
    webviewtest.h
   )
set( SOURCES
    webviewtest.cpp
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )

add_library( webviewtest SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( webviewtest ${QT_LIBRARIES} extensionsystem )

install_spec( webviewtest.spec )
