set( GLOBAL_HEADERS GuiSystem guisystem_global.h)
set( HEADERS
    actionmanager.h
    command.h
    commandcontainer.h
    proxyaction.h
   )
set( SOURCES
    actionmanager.cpp
    command.cpp
    commandcontainer.cpp
    proxyaction.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../widgets )

add_definitions( -DGUISYSTEM_LIBRARY )

add_library( guisystem SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( guisystem ${QT_LIBRARIES} widgets )

install_lib( guisystem )
