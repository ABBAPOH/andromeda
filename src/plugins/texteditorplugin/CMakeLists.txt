set( HEADERS
    texteditor_global.h
    plaintextdocument.h
    plaintextedit.h
    plaintexteditor.h
    texteditorplugin.h
   )
set( SOURCES
    plaintextdocument.cpp
    plaintextedit.cpp
    plaintexteditor.cpp
    texteditorplugin.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

set( TS_FILES
    translations/texteditorplugin_pt.ts
    translations/texteditorplugin_ru.ts
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DTEXTEDITOR_LIBRARY )

add_library( texteditorplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( texteditorplugin ${QT_LIBRARIES} libextensionsystem libguisystem )
install_plugin( texteditorplugin )
