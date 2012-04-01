set( GLOBAL_HEADERS GuiSystem guisystem_global.h)
set( HEADERS
    action.h
    actionmanager.h
    abstractcontainer.h
    abstracteditor.h
    abstracteditorfactory.h
    abstractview.h
    abstractviewfactory.h
    command.h
    commandcontainer.h
    commandsmodel.h
    commandsmodel_p.h
    commandssettingswidget.h
    editormanager.h
    findtoolbar.h
    history.h
    historyitem.h
    ifile.h
    ifind.h
    ihistory.h
    mainwindow.h
    mainwindow_p.h
    proxyaction.h
    proxyfile.h
    proxyhistory.h
    settingsdialog.h
    settingspage.h
    settingspagemanager.h
    stackedcontainer.h
    stackedcontainer_p.h
    stackedhistory.h
   )
set( SOURCES
    action.cpp
    actionmanager.cpp
    abstractcontainer.cpp
    abstracteditor.cpp
    abstracteditorfactory.cpp
    abstractview.cpp
    abstractviewfactory.cpp
    command.cpp
    commandcontainer.cpp
    commandsmodel.cpp
    commandssettingswidget.cpp
    editormanager.cpp
    findtoolbar.cpp
    history.cpp
    historyitem.cpp
    ifile.cpp
    ifind.cpp
    ihistory.cpp
    mainwindow.cpp
    proxyaction.cpp
    proxyfile.cpp
    proxyhistory.cpp
    settingsdialog.cpp
    settingspage.cpp
    settingspagemanager.cpp
    stackedcontainer.cpp
    stackedhistory.cpp
   )
set( FORMS
    commandssettingswidget.ui
   )
set( RESOURCES resources.qrc )
set( TS_FILES translations/guisystem_ru.ts translations/guisystem_pt.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DGUISYSTEM_LIBRARY )

add_library( guisystem SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( guisystem ${QT_LIBRARIES} widgets )

install_lib( guisystem )
