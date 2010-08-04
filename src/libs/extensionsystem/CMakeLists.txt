set( HEADERS
    extensionsystem_global.h
    iplugin.h
    iplugin_p.h
    pluginmanager.h
    pluginmanager_p.h
    pluginspec.h
    pluginspec_p.h
    pluginview.h
    pluginviewmodel.h
    pluginviewmodel_p.h
    fullpluginview.h
   )
set( SOURCES
    iplugin.cpp
    pluginmanager.cpp
    pluginspec.cpp
    pluginview.cpp
    pluginviewmodel.cpp
    fullpluginview.cpp
   )
set( FORMS
    pluginview.ui
    fullpluginview.ui
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DEXTENSIONSYSTEM_LIBRARY )

add_library( extensionsystem SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( extensionsystem ${QT_LIBRARIES} )

