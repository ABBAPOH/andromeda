set( GLOBAL_HEADERS bookmarks_global.h )
set( HEADERS
    bookmark.h
    bookmarkdialog.h
    bookmarksconstants.h
    bookmarksdocument.h
    bookmarkseditor.h
    bookmarksmenu.h
    bookmarksmodel.h
    bookmarksmodel_p.h
    bookmarkstoolbar.h
    bookmarksplugin.h
    bookmarkswidget.h
    bookmarkswidget_p.h
   )
set( SOURCES
    bookmark.cpp
    bookmarkdialog.cpp
    bookmarksdocument.cpp
    bookmarkseditor.cpp
    bookmarksmenu.cpp
    bookmarkstoolbar.cpp
    bookmarksmodel.cpp
    bookmarksplugin.cpp
    bookmarkswidget.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

set( TS_FILES translations/bookmarksplugin_ru.ts translations/bookmarksplugin_pt.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_definitions( -DBOOKMARKS_LIBRARY )

add_library( bookmarksplugin SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( bookmarksplugin ${QT_LIBRARIES} extensionsystem guisystem )
install_plugin( bookmarksplugin )
