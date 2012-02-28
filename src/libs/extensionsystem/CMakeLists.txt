add_subdirectory( plugineditor )

set( GLOBAL_HEADERS ExtensionSystem extensionsystem_global.h)
set( HEADERS
    errorsdialog.h
    fullpluginview.h
    iplugin.h
    mutablepluginspec.h
    options.h
    plugineditor.h
    pluginmanager.h
    pluginmanager_p.h
    pluginspec.h
    pluginspec_p.h
    pluginspecformathandler_p.h
    pluginview.h
    pluginviewmodel.h
    pluginviewmodel_p.h
    qobjectpool.h
    qobjectpool_p.h
   )
set( SOURCES
    errorsdialog.cpp
    fullpluginview.cpp
    iplugin.cpp
    mutablepluginspec.cpp
    options.cpp
    plugineditor.cpp
    pluginmanager.cpp
    pluginspec.cpp
    pluginspecformathandler.cpp
    pluginview.cpp
    pluginviewmodel.cpp
    qobjectpool.cpp
   )
set( FORMS
    pluginview.ui
    fullpluginview.ui
    plugineditor.ui
   )
set( TS_FILES translations/extensionsystem_ru.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DEXTENSIONSYSTEM_LIBRARY )

add_library( extensionsystem SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QM_FILES} )
target_link_libraries( extensionsystem ${QT_LIBRARIES} )

install_lib( extensionsystem )
