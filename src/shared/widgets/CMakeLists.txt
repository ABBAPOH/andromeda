set( HEADERS
    widgets_global.h
    minisplitter.h
    enteredlineedit.h
   )
set( SOURCES
    minisplitter.cpp
    enteredlineedit.cpp
   )
set( FORMS
   )
qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

add_definitions( -DWIDGETS_LIBRARY )

add_library( widgets STATIC ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( widgets ${QT_LIBRARIES} )

