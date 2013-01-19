set( CPP_HEADERS
    filemanager_global.h
)
set( HEADERS
    dualpanewidget.h
    dualpanewidget_p.h
    filecopyerrordialog.h
    filecopyreplacedialog.h
#    filecopytask.h
    filecopytask_p.h
    filecopydialog.h
    filecopydialog_p.h
    fileexplorerwidget.h
    fileinfodialog.h
    filemanagerconstants.h
    filemanagerhistory.h
    filemanagerhistory_p.h
    filemanagerhistoryitem.h
    filemanagerhistoryitem_p.h
    filemanagersettings.h
    filemanagersettings_p.h
    filemanagerwidget.h
    filemanagerwidget_p.h
    filesystemmanager.h
    filesystemmanager_p.h
    filesystemmodel.h
    filethumbnails.h
    navigationmodel.h
    navigationmodel_p.h
    navigationpanel.h
    navigationpanel_p.h
    navigationpanelsettings.h
    openwithmenu.h
    permissionswidget.h
   )
set( SOURCES
    dualpanewidget.cpp
    filecopyerrordialog.cpp
    filecopyreplacedialog.cpp
    filecopytask.cpp
    filecopydialog.cpp
    fileexplorerwidget.cpp
    fileinfodialog.cpp
    filemanagerhistory.cpp
    filemanagerhistoryitem.cpp
    filemanagersettings.cpp
    filemanagerwidget.cpp
    filesystemmanager.cpp
    filesystemmodel.cpp
    filethumbnails.cpp
    navigationmodel.cpp
    navigationpanel.cpp
    navigationpanelsettings.cpp
    openwithmenu.cpp
    permissionswidget.cpp
   )
set( FORMS
    filecopyerrordialog.ui
    filecopyreplacedialog.ui
    filecopywidget.ui
    permissionswidget.ui
   )
#set( RESOURCES resources.qrc )
set( TS_FILES translations/filemanager_ru.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_automoc( ${SOURCES} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DFILEMANAGER_LIBRARY )

add_library( libfilemanager SHARED ${CPP_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( libfilemanager ${QT_LIBRARIES} libwidgets libio )
install_lib( libfilemanager )
