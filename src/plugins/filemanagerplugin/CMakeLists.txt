set( HEADERS
    filemanagerdocument.h
    filemanagereditor.h
    filemanagereditor_p.h
    filemanagerplugin.h
    filemanagerplugin_global.h
    filesystemtoolmodel.h
    filesystemtoolwidget.h
    filesystemtoolwidget_p.h
    viewmodessettings.h
    globalsettings.h
    openwitheditormenu.h
   )
set( SOURCES
    filemanagerdocument.cpp
    filemanagereditor.cpp
    filemanagerplugin.cpp
    filesystemtoolmodel.cpp
    filesystemtoolwidget.cpp
    viewmodessettings.cpp
    globalsettings.cpp
    openwitheditormenu.cpp
   )
set( FORMS
    viewmodessettings.ui
    globalsettings.ui
   )

set( RESOURCES resources.qrc )

set( TS_FILES
    translations/filemanagerplugin_pt.ts
    translations/filemanagerplugin_ru.ts
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

qt4_automoc(
    filecopydialogmanager.cpp
    filecopytask.cpp
)

include_directories( ${PROJECT_SOURCE_DIR}/src/libs )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DFILEMANAGERPLUGIN_LIBRARY )

add_library( filemanagerplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( filemanagerplugin ${QT_LIBRARIES} libextensionsystem libguisystem libio libwidgets libfilemanager )
add_dependencies( filemanagerplugin plugineditor)
install_plugin( filemanagerplugin )
