set( HEADERS
    mainwindowplugin_global.h
    mainwindow.h
    mainwindowplugin.h
   )
set( SOURCES
    mainwindow.cpp
    mainwindowplugin.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtfilecopier )
include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin )
include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin/editormanager )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/widgets )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DMAINWINDOWPLUGIN_LIBRARY )
#add_definitions( -DQT_QTCOPYDIALOG_IMPORT )

add_library( mainwindowplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( mainwindowplugin ${QT_LIBRARIES} extensionsystem io coreplugin widgets )#qtfilecopier )

install_spec( mainwindowplugin.spec )
