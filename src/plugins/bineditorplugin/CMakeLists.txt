set( HEADERS
    bineditor_global.h
    binedit.h
    bineditor.h
    bineditordocument.h
    bineditorplugin.h
   )
set( SOURCES
    bineditor.cpp
    bineditordocument.cpp
    binedit.cpp
    bineditorplugin.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

set( TS_FILES
    translations/bineditorplugin_ru.ts
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DHELLOWORLD_LIBRARY )

add_library( bineditorplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( bineditorplugin ${QT_LIBRARIES} libextensionsystem libguisystem )
install_plugin( bineditorplugin )
