set( GLOBAL_HEADERS GuiSystem guisystem_global.h)
set( HEADERS
    actionmanager.h
    abstracteditor.h
    abstracteditorfactory.h
    abstractview.h
    abstractviewfactory.h
    command.h
    commandcontainer.h
    editormanager.h
    proxyaction.h
   )
set( SOURCES
    actionmanager.cpp
    abstracteditor.cpp
    abstracteditorfactory.cpp
    abstractview.cpp
    abstractviewfactory.cpp
    command.cpp
    commandcontainer.cpp
    editormanager.cpp
    proxyaction.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DGUISYSTEM_LIBRARY )

add_library( guisystem SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( guisystem ${QT_LIBRARIES} widgets )

install_lib( guisystem )
