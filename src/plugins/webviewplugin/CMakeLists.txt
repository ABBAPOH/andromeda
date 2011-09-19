set( QT_USE_QTWEBKIT TRUE )
include( ${QT_USE_FILE} )

set( HEADERS
    webviewplugin_global.h
    webvieweditor.h
    webviewplugin.h
    webviewsettingspage.h
)
set( SOURCES
    webvieweditor.cpp
    webviewplugin.cpp
    webviewsettingspage.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin/editormanager )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DWEBVIEWPLUGIN_LIBRARY )

add_library( webviewplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( webviewplugin ${QT_LIBRARIES} extensionsystem coreplugin )
add_dependencies( webviewplugin plugineditor)
install_plugin( webviewplugin )
