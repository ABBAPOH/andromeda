set( GLOBAL_HEADER GuiSystem)
set( HEADERS
    guisystem_global.h
    guicontroller.h
    iview.h
    iviewfactory.h
    mainwindow.h
    perspective.h
    perspectiveinstance.h
    viewoptions.h
   )
set( SOURCES
    guicontroller.cpp
    iview.cpp
    iviewfactory.cpp
    mainwindow.cpp
    perspective.cpp
    perspectiveinstance.cpp
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

