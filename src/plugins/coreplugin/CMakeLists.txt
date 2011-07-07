set( GLOBAL_HEADER CorePlugin )
set( HEADERS
    constants.h
    coreplugin_global.h
    core.h
    core_p.h
    coreplugin.h
    history.h
    historyitem.h
    ioptionspage.h
    imainview.h
    mainwindow.h
    mainwindow_p.h
    perspectivemanager.h
    tab.h
    settingsdialog.h
    settingsdialog_p.h
   )
set( SOURCES
    core.cpp
    coreplugin.cpp
    history.cpp
    historyitem.cpp
    ioptionspage.cpp
    imainview.cpp
    mainwindow.cpp
    perspectivemanager.cpp
    tab.cpp
    settingsdialog.cpp
   )
set( FORMS
   )

set( RESOURCES resources.qrc )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )

include_directories( ${PROJECT_SOURCE_DIR}/src/shared/widgets )

add_definitions( -DCOREPLUGIN_LIBRARY )

add_library( coreplugin SHARED ${GLOBAL_HEADER} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( coreplugin ${QT_LIBRARIES} extensionsystem guisystem widgets)

install_spec( coreplugin.spec )
