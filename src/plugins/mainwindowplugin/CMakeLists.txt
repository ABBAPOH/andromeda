set( HEADERS
    mainwindowplugin_global.h
    mainwindowplugin.h
    filecopydialogmanager.h
    filecopyreplacedialog.h
    filecopytask.h
    filecopywidget.h
   )
set( SOURCES
    mainwindowplugin.cpp
    filecopydialogmanager.cpp
    filecopyreplacedialog.cpp
    filecopytask.cpp
    filecopywidget.cpp
   )
set( FORMS
    filecopyreplacedialog.ui
    filecopywidget.ui
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/io )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtfilecopier )
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DMAINWINDOWPLUGIN_LIBRARY )

add_library( mainwindowplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( mainwindowplugin ${QT_LIBRARIES} extensionsystem io )

