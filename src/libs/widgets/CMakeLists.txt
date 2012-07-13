set( CPP_HEADERS
    widgets_global.h
    qcolorbutton.h
    qcolorbutton_p.h
)
set( HEADERS
    addressbar.h
    fancylineedit.h
    fancylineedit_p.h
    filterlineedit.h
    minisplitter.h
    modelmenu.h
    modeltoolbar.h
    outlinewidget.h
    outlinewidget_p.h
    shortcutedit.h
    windowsmenu.h
   )
set( SOURCES
    addressbar.cpp
    fancylineedit.cpp
    filterlineedit.cpp
    minisplitter.cpp
    modelmenu.cpp
    modeltoolbar.cpp
    outlinewidget.cpp
    qcolorbutton.cpp
    shortcutedit.cpp
    windowsmenu.cpp
   )
set( FORMS
   )
set( RESOURCES resources.qrc )
set( TS_FILES translations/widgets_ru.ts translations/widgets_pt.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_automoc( ${SOURCES} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DWIDGETS_LIBRARY )

add_library( widgets SHARED ${CPP_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( widgets ${QT_LIBRARIES} )
install_lib( widgets )
