set( GLOBAL_HEADER FilemanagerPlugin )

set( HEADERS
    filemanagerplugin_global.h
    dualpanewidget.h
    filecopyerrordialog.h
    filecopyreplacedialog.h
#    filecopytask.h
    filecopytask_p.h
    filecopydialog.h
    filecopydialog_p.h
    fileinfodialog.h
    filemanagereditor.h
    filemanagerplugin.h
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
    filemanagerwidget.cpp
    filesystemmodel.cpp
    filesystemmanager.cpp
    filemanagersettingspage.cpp
    filemanagersettingswidget.cpp
    navigationmodel.cpp
    navigationpanel.cpp
   )
set( FORMS
    filecopyerrordialog.ui
    filecopyreplacedialog.ui
    filecopywidget.ui
    fileinfodialog.ui
    filemanagersettingswidget.ui
   )

set( RESOURCES resources.qrc )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )

qt4_automoc(
    filecopydialogmanager.cpp
    filecopytask.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtfilecopier )
include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/mainwindowplugin )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/widgets )

add_definitions( -DFILEMANAGERPLUGIN_LIBRARY )

add_library( filemanagerplugin SHARED ${GLOBAL_HEADER} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( filemanagerplugin ${QT_LIBRARIES} extensionsystem guisystem io coreplugin widgets )
add_dependencies( filemanagerplugin plugineditor)
install_plugin( filemanagerplugin )
