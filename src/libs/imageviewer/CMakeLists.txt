set( QT_USE_QTOPENGL TRUE )
include( ${QT_USE_FILE} )

set( CPP_HEADERS
    imageviewer_global.h
    imageview.h
)
set( HEADERS
    imageview_p.h
    imageviewsettings.h
    imageviewsettings_p.h
    preferenceswidget.h
    resizedialog.h
   )
set( SOURCES
    imageview.cpp
    imageviewsettings.cpp
    preferenceswidget.cpp
    resizedialog.cpp
   )
set( FORMS
    preferenceswidget.ui
    resizedialog.ui
   )
#set( RESOURCES resources.qrc )
set( TS_FILES
    translations/imageviewer_ru.ts
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_automoc( ${SOURCES} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DIMAGEVIEWER_LIBRARY )

add_library( libimageviewer SHARED ${CPP_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( libimageviewer ${QT_LIBRARIES} libwidgets )
install_lib( libimageviewer )
