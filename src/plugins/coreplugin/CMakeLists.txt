set( GLOBAL_HEADER CorePlugin )
set( HEADERS
    abstracteditor.h
    abstracteditorfactory.h
    constants.h
    coreplugin_global.h
    core.h
    core_p.h
    coreplugin.h
    editormanager.h
    history.h
    historyitem.h
    isettingspage.h
    mainwindow.h
    mainwindow_p.h
    settings.h
    settingsdialog.h
    settingspagemanager.h
    tab.h
    tabbar.h
   )
set( SOURCES
    abstracteditor.cpp
    abstracteditorfactory.cpp
    core.cpp
    coreplugin.cpp
    editormanager.cpp
    history.cpp
    historyitem.cpp
    isettingspage.cpp
    mainwindow.cpp
    settings.cpp
    settingsdialog.cpp
    settingspagemanager.cpp
    tab.cpp
    tabbar.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/widgets )

add_definitions( -DCOREPLUGIN_LIBRARY )

add_library( coreplugin SHARED ${GLOBAL_HEADER} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( coreplugin ${QT_LIBRARIES} extensionsystem guisystem widgets)
add_dependencies( coreplugin plugineditor)
install_plugin( coreplugin )
