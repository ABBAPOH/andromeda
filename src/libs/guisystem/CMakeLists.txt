set( GLOBAL_HEADERS GuiSystem guisystem_global.h)
set( HEADERS
    actionmanager.h
    actionmanager_p.h
    centralwidget.h
    command.h
    commandcontainer.h
    guicontroller.h
    iview.h
    iviewfactory.h
    iiohandler.h
#    mainwindow.h
    menubar.h
    perspective.h
    perspectiveinstance.h
#    perspectiveswitcher.h
    perspectivewidget.h
#    state.h
    viewoptions.h
   )
set( SOURCES
    actionmanager.cpp
    centralwidget.cpp
    command.cpp
    commandcontainer.cpp
    guicontroller.cpp
    iview.cpp
    iviewfactory.cpp
    iiohandler.cpp
#    mainwindow.cpp
    menubar.cpp
    perspective.cpp
    perspectiveinstance.cpp
#    perspectiveswitcher.cpp
    perspectivewidget.cpp
#    state.cpp
    viewoptions.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../../shared/widgets )

add_definitions( -DGUISYSTEM_LIBRARY )

add_library( guisystem SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( guisystem ${QT_LIBRARIES} widgets )

