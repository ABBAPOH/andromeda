set( QT_USE_QTNETWORK TRUE )
include( ${QT_USE_FILE} )

set( GLOBAL_HEADERS guisystem_global.h)
set( HEADERS
    core/abstractdocument.h
    core/abstractdocument_p.h
    core/abstractdocumentfactory.h
    core/documentmanager.h
    core/filedocument.h
    core/filedocument_p.h
    core/history.h
    core/historyitem.h
    core/ifind.h
    core/ihistory.h
    core/toolmodel.h
    core/sharedproperties.h
    core/sharedproperties_p.h
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
    widgets/toolwidget.h
    widgets/toolwidgetfactory.h
    widgets/toolwidgetmanager.h
    actionmanager/abstractcommand.h
    actionmanager/abstractcommand_p.h
    actionmanager/actionmanager.h
    actionmanager/command.h
    actionmanager/commandcontainer.h
    actionmanager/commandsmodel.h
    actionmanager/commandsmodel_p.h
    actionmanager/commandssettingswidget.h
    actionmanager/commandssettingswidget_p.h
    actionmanager/proxyaction.h
    actionmanager/standardcommands.h
    actionmanager/standardcommands_p.h
    constants.h
    settingspage.h
    settingspagemanager.h
    settingswindow.h
   )
set( SOURCES
    core/abstractdocument.cpp
    core/abstractdocumentfactory.cpp
    core/documentmanager.cpp
    core/filedocument.cpp
    core/history.cpp
    core/historyitem.cpp
    core/ifind.cpp
    core/ihistory.cpp
    core/toolmodel.cpp
    core/sharedproperties.cpp
    widgets/abstracteditor.cpp
    widgets/abstracteditorfactory.cpp
    widgets/editormanager.cpp
    widgets/editorviewhistory.cpp
    widgets/editorview.cpp
    widgets/editorwindow.cpp
    widgets/editorwindowfactory.cpp
    widgets/findtoolbar.cpp
    widgets/historybutton.cpp
    widgets/toolwidget.cpp
    widgets/toolwidgetfactory.cpp
    widgets/toolwidgetmanager.cpp
    actionmanager/abstractcommand.cpp
    actionmanager/actionmanager.cpp
    actionmanager/command.cpp
    actionmanager/commandcontainer.cpp
    actionmanager/commandsmodel.cpp
    actionmanager/commandssettingswidget.cpp
    actionmanager/proxyaction.cpp
    actionmanager/standardcommands.cpp
    settingspage.cpp
    settingspagemanager.cpp
    settingswindow.cpp
   )
set( FORMS
    actionmanager/commandssettingswidget.ui
   )
set( RESOURCES guisystem.qrc )
set( TS_FILES
    translations/guisystem_cs.ts
    translations/guisystem_pt.ts
    translations/guisystem_ru.ts
   )

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
