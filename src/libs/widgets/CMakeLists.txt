set( HEADERS
    widgets_global.h
    addressbar.h
    coverflow.h
    fancylineedit.h
    fancylineedit_p.h
    minisplitter.h
    modelmenu.h
    modeltoolbar.h
    enteredlineedit.h
    qxtflowview.h
    qxtflowview_p.h
   )
set( SOURCES
    addressbar.cpp
    coverflow.cpp
    fancylineedit.cpp
    minisplitter.cpp
    modelmenu.cpp
    modeltoolbar.cpp
    enteredlineedit.cpp
    qxtflowview.cpp
    qxtflowview_p.cpp
   )
set( FORMS
   )
set( RESOURCES resources.qrc )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )

add_definitions( -DWIDGETS_LIBRARY )

add_library( widgets SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( widgets ${QT_LIBRARIES} )
install_lib( widgets )
