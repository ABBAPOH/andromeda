set( HEADERS
    filemanagerplugin_global.h
    FilemanagerPlugin
 #   filecopydialogmanager.h
    filecopydialogmanager_p.h
    filecopyreplacedialog.h
#    filecopytask.h
    filecopytask_p.h
    filecopywidget.h
    filecopydialog.h
    filemanagerwidget.h
    filemanagerwidget_p.h
    filemanagerview.h
    filesystemmodel.h
    filesystemundomanager.h
    navigationmodel.h
    navigationmodel_p.h
    navigationpanel.h
   )
set( SOURCES
    filecopydialogmanager.cpp
    filecopyreplacedialog.cpp
    filecopytask.cpp
    filecopywidget.cpp
    filecopydialog.cpp
    filemanagerwidget.cpp
    filemanagerview.cpp
    filesystemmodel.cpp
    filesystemundomanager.cpp
    navigationmodel.cpp
    navigationpanel.cpp
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

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/guisystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtfilecopier )
#include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin )
include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin/editormanager )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/widgets )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DFILEMANAGERPLUGIN_LIBRARY )

add_library( filemanagerplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( filemanagerplugin ${QT_LIBRARIES} extensionsystem guisystem io )

install_spec( filemanagerplugin.spec )
