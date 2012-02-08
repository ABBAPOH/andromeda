set( GLOBAL_HEADER FilemanagerPlugin )

set( HEADERS
    filemanagerplugin_global.h
    dualpanewidget.h
    dualpanewidget_p.h
    filecopyerrordialog.h
    filecopyreplacedialog.h
#    filecopytask.h
    filecopytask_p.h
    filecopydialog.h
    filecopydialog_p.h
    fileinfodialog.h
    filemanagerconstants.h
    filemanagereditor.h
    filemanagereditor_p.h
    filemanagerplugin.h
    filemanagersettings.h
    filemanagersettings_p.h
    filemanagerwidget.h
    filemanagerwidget_p.h
    filemanagersettingspage.h
    filemanagersettingswidget.h
    filesystemmodel.h
    filesystemmanager.h
    filesystemmanager_p.h
    navigationmodel.h
    navigationmodel_p.h
    navigationpanel.h
    navigationpanel_p.h
    navigationpanelsettings.h
   )
set( SOURCES
    dualpanewidget.cpp
    filecopyerrordialog.cpp
    filecopyreplacedialog.cpp
    filecopytask.cpp
    filecopydialog.cpp
    fileinfodialog.cpp
    filemanagereditor.cpp
    filemanagerplugin.cpp
    filemanagersettings.cpp
    filemanagerwidget.cpp
    filesystemmodel.cpp
    filesystemmanager.cpp
    filemanagersettingspage.cpp
    filemanagersettingswidget.cpp
    navigationmodel.cpp
    navigationpanel.cpp
    navigationpanelsettings.cpp
   )
set( FORMS
    filecopyerrordialog.ui
    filecopyreplacedialog.ui
    filecopywidget.ui
    fileinfodialog.ui
    filemanagersettingswidget.ui
   )

set( RESOURCES resources.qrc )

set( TS_FILES translations/filemanagerplugin_ru.ts )

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

add_definitions( -DFILEMANAGERPLUGIN_LIBRARY )

add_library( filemanagerplugin SHARED ${GLOBAL_HEADER} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( filemanagerplugin ${QT_LIBRARIES} extensionsystem guisystem io widgets )
add_dependencies( filemanagerplugin plugineditor)
install_plugin( filemanagerplugin )
