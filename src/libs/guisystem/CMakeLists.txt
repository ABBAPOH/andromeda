set( QT_USE_QTNETWORK TRUE )
include( ${QT_USE_FILE} )

set( GLOBAL_HEADERS guisystem_global.h)
set( HEADERS
    action.h
    actionmanager.h
    abstractdocument.h
    abstractdocument_p.h
    abstracteditor.h
    abstracteditorfactory.h
    command.h
    commandcontainer.h
    commandsmodel.h
    commandsmodel_p.h
    commandssettingswidget.h
    constants.h
    editormanager.h
    editorviewhistory.h
    editorviewhistory_p.h
    editorview.h
    editorview_p.h
    editorwindow.h
    editorwindow_p.h
    editorwindowfactory.h
    filedocument.h
    filedocument_p.h
    findtoolbar.h
    history.h
    historybutton.h
    historyitem.h
    ifind.h
    ihistory.h
    menubarcontainer.h
    menubarcontainer_p.h
    proxyaction.h
    proxyhistory.h
    settingspage.h
    settingspagemanager.h
    settingswindow.h
    sharedproperties.h
    sharedproperties_p.h
   )
set( SOURCES
    action.cpp
    actionmanager.cpp
    abstractdocument.cpp
    abstracteditor.cpp
    abstracteditorfactory.cpp
    command.cpp
    commandcontainer.cpp
    commandsmodel.cpp
    commandssettingswidget.cpp
    editormanager.cpp
    editorviewhistory.cpp
    editorview.cpp
    editorwindow.cpp
    editorwindowfactory.cpp
    filedocument.cpp
    findtoolbar.cpp
    history.cpp
    historybutton.cpp
    historyitem.cpp
    ifind.cpp
    ihistory.cpp
    menubarcontainer.cpp
    proxyaction.cpp
    proxyhistory.cpp
    settingspage.cpp
    settingspagemanager.cpp
    settingswindow.cpp
    sharedproperties.cpp
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
target_link_libraries( guisystem ${QT_LIBRARIES} widgets io )

install_lib( guisystem )
