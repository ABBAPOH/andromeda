set( HEADERS
    imageviewerplugin_global.h
    imageviewerdocument.h
    imageviewereditor.h
    imageviewerplugin.h
   )
set( SOURCES
    imageviewerdocument.cpp
    imageviewereditor.cpp
    imageviewerplugin.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

set( TS_FILES
    translations/imageviewerplugin_ru.ts
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DIMAGEVIEWERPLUGIN_LIBRARY )

add_library( imageviewerplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( imageviewerplugin ${QT_LIBRARIES} libextensionsystem libguisystem libimageviewer )
install_plugin( imageviewerplugin )
