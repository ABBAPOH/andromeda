set( HEADERS
    FilemanagerPlugin
    filemanagerplugin_global.h
    filemanagerwidget.h
    filesystemmodel.h
    filesystemundomanager.h
    navigationmodel.h
    navigationmodel_p.h
    navigationpanel.h
   )
set( SOURCES
    filemanagerwidget.cpp
    filesystemmodel.cpp
    filesystemundomanager.cpp
    navigationmodel.cpp
    navigationpanel.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )

qt4_automoc(
)

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
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
