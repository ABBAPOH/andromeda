add_subdirectory( plugineditor )

set( GLOBAL_HEADERS ExtensionSystem extensionsystem_global.h)
set( HEADERS
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
    fullpluginview.h
    qobjectpool.h
    qobjectpool_p.h
   )
set( SOURCES
    iplugin.cpp
    mutablepluginspec.cpp
    options.cpp
    plugineditor.cpp
    pluginmanager.cpp
    pluginspec.cpp
    pluginspecformathandler.cpp
    pluginview.cpp
    pluginviewmodel.cpp
    fullpluginview.cpp
    qobjectpool.cpp
   )
set( FORMS
    pluginview.ui
    fullpluginview.ui
    plugineditor.ui
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DEXTENSIONSYSTEM_LIBRARY )

add_library( extensionsystem SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( extensionsystem ${QT_LIBRARIES} )

install_lib( extensionsystem )
