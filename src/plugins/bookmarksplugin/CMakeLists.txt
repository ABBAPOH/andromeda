set( GLOBAL_HEADERS bookmarksplugin_global.h )

set( HEADERS
    bookmarksconstants.h
    bookmarkseditor.h
    bookmarksplugin.h
   )
set( SOURCES
    bookmarkseditor.cpp
    bookmarksplugin.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

set( TS_FILES translations/bookmarksplugin_ru.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DBOOKMARKSPLUGIN_LIBRARY )

add_library( bookmarksplugin SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( bookmarksplugin ${QT_LIBRARIES} extensionsystem guisystem bookmarks coreplugin )
install_plugin( bookmarksplugin )
