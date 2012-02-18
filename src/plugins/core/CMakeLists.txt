set( HEADERS
    addressbar.h
    browserwindow.h
    browserwindow_p.h
    commandssettingspage.h
    constants.h
    core_global.h
    coreplugin.h
    settingsmodel.h
    settingsmodel_p.h
    settingswidget.h
    tabbar.h
    tabcontainer.h
    tabwidget.h
   )
set( SOURCES
    addressbar.cpp
    browserwindow.cpp
    commandssettingspage.cpp
    coreplugin.cpp
    settingsmodel.cpp
    settingswidget.cpp
    tabbar.cpp
    tabcontainer.cpp
    tabwidget.cpp
   )
set( FORMS
    settingswidget.ui
   )

set( RESOURCES resources.qrc )

set( TS_FILES translations/coreplugin_ru.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

add_definitions( -DCORE_LIBRARY )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_library( coreplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES})
target_link_libraries( coreplugin ${QT_LIBRARIES} extensionsystem guisystem widgets)
add_dependencies( coreplugin plugineditor)
install_plugin( coreplugin )
