set( QT_USE_QTOPENGL TRUE )
include( ${QT_USE_FILE} )

set( CPP_HEADERS
    qimageview_global.h
    qimageview.h
)
set( HEADERS
    qimageview_p.h
    qimageviewpreferenceswidget.h
    qimageviewresizedialog.h
    qimageviewsettings.h
    qimageviewsettings_p.h
   )
set( SOURCES
    qimageview.cpp
    qimageviewpreferenceswidget.cpp
    qimageviewresizedialog.cpp
    qimageviewsettings.cpp
   )
set( FORMS
    qimageviewpreferenceswidget.ui
    qimageviewresizedialog.ui
   )
#set( RESOURCES resources.qrc )
#set( TS_FILES translations/qimageview_ru.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_automoc( ${SOURCES} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DQIMAGEVIEW_LIBRARY )

add_library( qimageview SHARED ${CPP_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
target_link_libraries( qimageview ${QT_LIBRARIES} widgets )
install_lib( qimageview )
