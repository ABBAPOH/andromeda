set( HEADERS
    widgets_global.h
    minisplitter.h
   )
set( SOURCES
    minisplitter.cpp
   )

add_library( widgets STATIC ${HEADERS} ${SOURCES} )
target_link_libraries( widgets ${QT_LIBRARIES} )

