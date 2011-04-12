set( GLOBAL_HEADER GuiSystem)
set( HEADERS
    actionmanager.h
    actionmanager_p.h
    guisystem_global.h
    centralwidget.h
    guicontroller.h
    iview.h
    iviewfactory.h
    iiohandler.h
    mainwindow.h
    perspective.h
    perspective_p.h
    perspectiveinstance.h
    perspectiveswitcher.h
    state.h
    viewoptions.h
   )
set( SOURCES
    actionmanager.cpp
    centralwidget.cpp
    guicontroller.cpp
    iview.cpp
    iviewfactory.cpp
    iiohandler.cpp
    mainwindow.cpp
    perspective.cpp
    perspectiveinstance.cpp
    perspectiveswitcher.cpp
    state.cpp
    viewoptions.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DGUISYSTEM_LIBRARY )

add_library( guisystem SHARED ${GLOBAL_HEADER} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( guisystem ${QT_LIBRARIES} )

