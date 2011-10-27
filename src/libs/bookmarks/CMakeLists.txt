set( GLOBAL_HEADERS bookmarks_global.h )
set( HEADERS
    bookmark.h
   )
set( SOURCES
    bookmark.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DBOOKMARKS_LIBRARY )

add_library( bookmarks SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( bookmarks ${QT_LIBRARIES} )

install_lib( bookmarks )
