set( HEADERS
    filemanager_global.h
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
    filemanagereditor.h
    filemanagereditor_p.h
    filemanagerhistory.h
    filemanagerhistoryitem.h
    filemanagerplugin.h
    filemanagersettings.h
    filemanagersettings_p.h
    filemanagerwidget.h
    filemanagerwidget_p.h
    filethumbnails.h
    viewmodessettings.h
    globalsettings.h
    filesystemmodel.h
    filesystemmanager.h
    filesystemmanager_p.h
    navigationmodel.h
    navigationmodel_p.h
    navigationpanel.h
    navigationpanel_p.h
    navigationpanelsettings.h
    openwithmenu.h
    openwitheditormenu.h
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
    filemanagereditor.cpp
    filemanagerhistory.cpp
    filemanagerhistoryitem.cpp
    filemanagerplugin.cpp
    filemanagersettings.cpp
    filemanagerwidget.cpp
    filesystemmodel.cpp
    filesystemmanager.cpp
    filethumbnails.cpp
    viewmodessettings.cpp
    globalsettings.cpp
    navigationmodel.cpp
    navigationpanel.cpp
    navigationpanelsettings.cpp
    openwithmenu.cpp
    openwitheditormenu.cpp
    permissionswidget.cpp
   )
set( FORMS
    filecopyerrordialog.ui
    filecopyreplacedialog.ui
    filecopywidget.ui
    viewmodessettings.ui
    globalsettings.ui
    permissionswidget.ui
   )

set( RESOURCES resources.qrc )

set( TS_FILES translations/filemanagerplugin_ru.ts translations/filemanagerplugin_pt.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

qt4_automoc(
    filecopydialogmanager.cpp
    filecopytask.cpp
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
#include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )

add_definitions( -DFILEMANAGER_LIBRARY )

add_library( filemanagerplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( filemanagerplugin ${QT_LIBRARIES} extensionsystem guisystem io widgets )
add_dependencies( filemanagerplugin plugineditor)
install_plugin( filemanagerplugin )
