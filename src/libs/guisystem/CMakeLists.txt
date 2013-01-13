set( QT_USE_QTNETWORK TRUE )
include( ${QT_USE_FILE} )

set( GLOBAL_HEADERS guisystem_global.h)
set( HEADERS
    core/abstractdocument.h
    core/abstractdocument_p.h
    core/filedocument.h
    core/filedocument_p.h
    core/history.h
    core/historyitem.h
    core/ifind.h
    core/ihistory.h
    widgets/abstracteditor.h
    widgets/abstracteditorfactory.h
    widgets/editormanager.h
    widgets/editorviewhistory.h
    widgets/editorviewhistory_p.h
    widgets/editorview.h
    widgets/editorview_p.h
    widgets/editorwindow.h
    widgets/editorwindow_p.h
    widgets/editorwindowfactory.h
    widgets/findtoolbar.h
    widgets/historybutton.h
    action.h
    actionmanager.h
    command.h
    commandcontainer.h
    commandsmodel.h
    commandsmodel_p.h
    commandssettingswidget.h
    commandssettingswidget_p.h
    constants.h
    menubarcontainer.h
    menubarcontainer_p.h
    proxyaction.h
    settingspage.h
    settingspagemanager.h
    settingswindow.h
    sharedproperties.h
    sharedproperties_p.h
   )
set( SOURCES
    core/abstractdocument.cpp
    core/filedocument.cpp
    core/history.cpp
    core/historyitem.cpp
    core/ifind.cpp
    core/ihistory.cpp
    widgets/abstracteditor.cpp
    widgets/abstracteditorfactory.cpp
    widgets/editormanager.cpp
    widgets/editorviewhistory.cpp
    widgets/editorview.cpp
    widgets/editorwindow.cpp
    widgets/editorwindowfactory.cpp
    widgets/findtoolbar.cpp
    widgets/historybutton.cpp
    action.cpp
    actionmanager.cpp
    command.cpp
    commandcontainer.cpp
    commandsmodel.cpp
    commandssettingswidget.cpp
    menubarcontainer.cpp
    proxyaction.cpp
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
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/core )

add_definitions( -DGUISYSTEM_LIBRARY )

add_library( libguisystem SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( libguisystem ${QT_LIBRARIES} libwidgets libio )

install_lib( libguisystem )
