set( HEADERS
    widgets_global.h
    coverflow.h
    minisplitter.h
    enteredlineedit.h
    qxtflowview.h
    qxtflowview_p.h
   )
set( SOURCES
    coverflow.cpp
    minisplitter.cpp
    enteredlineedit.cpp
    qxtflowview.cpp
    qxtflowview_p.cpp
   )
set( FORMS
   )
qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

add_definitions( -DWIDGETS_LIBRARY )

add_library( widgets SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( widgets ${QT_LIBRARIES} )
install_lib( widgets )
