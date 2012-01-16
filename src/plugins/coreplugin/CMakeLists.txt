set( GLOBAL_HEADER CorePlugin )
set( HEADERS
    browserwindow.h
    browserwindow_p.h
    constants.h
    coreplugin_global.h
    coreplugin.h
    isettingspage.h
    settingsdialog.h
    settingsmodel.h
    settingsmodel_p.h
    settingswidget.h
    settingspagemanager.h
    tabbar.h
   )
set( SOURCES
    browserwindow.cpp
    coreplugin.cpp
    isettingspage.cpp
    settingsdialog.cpp
    settingsmodel.cpp
    settingswidget.cpp
    settingspagemanager.cpp
    tabbar.cpp
   )
set( FORMS
    settingswidget.ui
   )

set( RESOURCES resources.qrc )

set( TS_FILES translations/coreplugin_ru.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

add_definitions( -DCOREPLUGIN_LIBRARY )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_library( coreplugin SHARED ${GLOBAL_HEADER} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES})
target_link_libraries( coreplugin ${QT_LIBRARIES} extensionsystem guisystem widgets)
add_dependencies( coreplugin plugineditor)
install_plugin( coreplugin )
