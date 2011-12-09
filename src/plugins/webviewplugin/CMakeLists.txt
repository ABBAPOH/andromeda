set( QT_USE_QTWEBKIT TRUE )
include( ${QT_USE_FILE} )

set( HEADERS
    appearancesettings.h
    cookiejar.h
    cookiedialog.h
    cookiemodel.h
    cookieexceptionsdialog.h
    cookieexceptionsmodel.h
    privacysettings.h
    proxysettings.h
    webviewplugin_global.h
    webvieweditor.h
    webvieweditor_p.h
    webviewplugin.h
    webviewsettingspage.h
)
set( SOURCES
    appearancesettings.cpp
    cookiejar.cpp
    cookiedialog.cpp
    cookiemodel.cpp
    cookieexceptionsdialog.cpp
    cookieexceptionsmodel.cpp
    privacysettings.cpp
    proxysettings.cpp
    webvieweditor.cpp
    webviewplugin.cpp
    webviewsettingspage.cpp
   )
set( FORMS
    appearancesettings.ui
    cookiedialog.ui
    cookieexceptionsdialog.ui
    privacysettings.ui
    proxysettings.ui
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DWEBVIEWPLUGIN_LIBRARY )

add_library( webviewplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( webviewplugin ${QT_LIBRARIES} extensionsystem coreplugin )
add_dependencies( webviewplugin plugineditor)
install_plugin( webviewplugin )
