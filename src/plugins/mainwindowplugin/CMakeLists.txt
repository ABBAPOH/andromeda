set( HEADERS
    mainwindowplugin_global.h
    mainwindowplugin.h
    filecopydialogmanager.h
    filecopyreplacedialog.h
    filecopytask.h
    filecopywidget.h
    filecopydialog.h
   )
set( SOURCES
    mainwindowplugin.cpp
    filecopydialogmanager.cpp
    filecopyreplacedialog.cpp
    filecopytask.cpp
    filecopywidget.cpp
    filecopydialog.cpp
   )
set( FORMS
    filecopyreplacedialog.ui
    filecopywidget.ui
   )

set( RESOURCES resources.qrc )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtfilecopier )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DMAINWINDOWPLUGIN_LIBRARY )

add_library( mainwindowplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( mainwindowplugin ${QT_LIBRARIES} extensionsystem io )

