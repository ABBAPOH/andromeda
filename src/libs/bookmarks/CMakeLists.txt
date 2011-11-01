set( GLOBAL_HEADERS bookmarks_global.h )
set( HEADERS
    bookmark.h
    bookmarkdialog.h
    bookmarksmodel.h
    bookmarksmodel_p.h
    bookmarkswidget.h
    bookmarkswidget_p.h
   )
set( SOURCES
    bookmark.cpp
    bookmarkdialog.cpp
    bookmarksmodel.cpp
    bookmarkswidget.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../widgets)

add_definitions( -DBOOKMARKS_LIBRARY )

add_library( bookmarks SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( bookmarks ${QT_LIBRARIES} widgets )

install_lib( bookmarks )
