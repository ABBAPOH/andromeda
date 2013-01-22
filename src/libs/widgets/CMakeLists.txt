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
    shortcutedit_p.h
    tabbar.h
    tabbarbutton.h
    tabwidget.h
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
    tabbar.cpp
    tabbarbutton.cpp
    tabwidget.cpp
    windowsmenu.cpp
   )
set( FORMS
   )
set( RESOURCES widgets.qrc )
set( TS_FILES
    translations/widgets_cs.ts
    translations/widgets_pt.ts
    translations/widgets_ru.ts
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_automoc( ${SOURCES} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DWIDGETS_LIBRARY )

add_library( libwidgets SHARED ${CPP_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( libwidgets ${QT_LIBRARIES} )
install_lib( libwidgets )
