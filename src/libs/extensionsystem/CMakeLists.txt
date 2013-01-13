add_subdirectory( plugineditor )

set( GLOBAL_HEADERS extensionsystem_global.h)
set( HEADERS
    core/iplugin.h
    core/mutablepluginspec.h
    core/pluginmanager.h
    core/pluginmanager_p.h
    core/pluginspec.h
    core/pluginspec_p.h
    core/pluginspecformathandler_p.h
    core/qobjectpool.h
    core/qobjectpool_p.h
    core/options.h
    core/pluginviewmodel.h
    core/pluginviewmodel_p.h
    widgets/errorsdialog.h
    widgets/fullpluginview.h
    widgets/plugineditor.h
    widgets/pluginview.h
   )
set( SOURCES
    core/iplugin.cpp
    core/mutablepluginspec.cpp
    core/options.cpp
    core/pluginmanager.cpp
    core/pluginspec.cpp
    core/pluginspecformathandler.cpp
    core/pluginviewmodel.cpp
    core/qobjectpool.cpp
    widgets/errorsdialog.cpp
    widgets/fullpluginview.cpp
    widgets/plugineditor.cpp
    widgets/pluginview.cpp
   )
set( FORMS
    widgets/pluginview.ui
    widgets/fullpluginview.ui
    widgets/plugineditor.ui
   )
set( TS_FILES translations/extensionsystem_ru.ts translations/extensionsystem_pt.ts )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_wrap_ts( QM_FILES ${TS_FILES} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DEXTENSIONSYSTEM_LIBRARY )

add_library( libextensionsystem SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QM_FILES} )
target_link_libraries( libextensionsystem ${QT_LIBRARIES} )

install_lib( libextensionsystem )
