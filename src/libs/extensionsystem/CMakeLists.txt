set( HEADERS
    extensionsystem_global.h
    iplugin.h
    iplugin_p.h
    pluginmanager.h
    pluginmanager_p.h
    pluginspec.h
    pluginspec_p.h
   )
set( SOURCES
    iplugin.cpp
    pluginmanager.cpp
    pluginspec.cpp
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

add_definitions( -DEXTENSIONSYSTEM_LIBRARY )

add_library( extensionsystem SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( extensionsystem ${QT_LIBRARIES} )
