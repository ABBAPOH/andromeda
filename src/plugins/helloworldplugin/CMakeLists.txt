set( HEADERS
    helloworld_global.h
    helloworlddocument.h
    helloworldeditor.h
    helloworldplugin.h
   )
set( SOURCES
    helloworlddocument.cpp
    helloworldeditor.cpp
    helloworldplugin.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

set( TS_FILES
    translations/helloworldplugin_pt.ts
    translations/helloworldplugin_ru.ts
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DHELLOWORLD_LIBRARY )

add_library( helloworldplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( helloworldplugin ${QT_LIBRARIES} libextensionsystem libguisystem )
install_plugin( helloworldplugin )
