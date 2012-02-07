set( GLOBAL_HEADERS GuiSystem guisystem_global.h)
set( HEADERS
    action.h
    actionmanager.h
    abstractcontainer.h
    abstracteditor.h
    abstracteditorfactory.h
    abstracthistory.h
    abstractview.h
    abstractviewfactory.h
    command.h
    commandcontainer.h
    commandsmodel.h
    commandsmodel_p.h
    editormanager.h
    findtoolbar.h
    history.h
    historyitem.h
    ifind.h
    mainwindow.h
    mainwindow_p.h
    proxyaction.h
    proxyhistory.h
    settingsdialog.h
    settingspage.h
    settingspagemanager.h
    stackedcontainer.h
    stackedcontainer_p.h
   )
set( SOURCES
    action.cpp
    actionmanager.cpp
    abstractcontainer.cpp
    abstracteditor.cpp
    abstracteditorfactory.cpp
    abstracthistory.cpp
    abstractview.cpp
    abstractviewfactory.cpp
    command.cpp
    commandcontainer.cpp
    commandsmodel.cpp
    editormanager.cpp
    findtoolbar.cpp
    history.cpp
    historyitem.cpp
    ifind.cpp
    mainwindow.cpp
    proxyaction.cpp
    proxyhistory.cpp
    settingsdialog.cpp
    settingspage.cpp
    settingspagemanager.cpp
    stackedcontainer.cpp
   )
set( FORMS
   )
set( RESOURCES resources.qrc )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DGUISYSTEM_LIBRARY )

add_library( guisystem SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( guisystem ${QT_LIBRARIES} widgets )

install_lib( guisystem )
