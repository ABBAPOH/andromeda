set( HEADERS
    filemanagerplugin_global.h
    FilemanagerPlugin
    dualpanewidget.h
    filecopyreplacedialog.h
#    filecopytask.h
    filecopytask_p.h
    filecopydialog.h
    filecopydialog_p.h
    filemanagerpluginimpl.h
    filemanagerview.h
    filemanagerwidget.h
    filemanagerwidget_p.h
    filesystemmodel.h
    filesystemmanager.h
    navigationmodel.h
    navigationmodel_p.h
    navigationpanel.h
    navigationpanelview.h
   )
set( SOURCES
    dualpanewidget.cpp
    filecopyreplacedialog.cpp
    filecopytask.cpp
    filecopydialog.cpp
    filemanagerpluginimpl.cpp
    filemanagerview.cpp
    filemanagerwidget.cpp
    filesystemmodel.cpp
    filesystemmanager.cpp
    navigationmodel.cpp
    navigationpanel.cpp
    navigationpanelview.cpp
   )
set( FORMS
    filecopyreplacedialog.ui
    filecopywidget.ui
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

add_definitions( -DFILEMANAGERPLUGIN_LIBRARY )

add_library( filemanagerplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( filemanagerplugin ${QT_LIBRARIES} extensionsystem guisystem io coreplugin )

install_spec( filemanagerplugin.spec )
