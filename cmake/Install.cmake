include("cmake/LibSuffix.cmake")

function( install_spec SPECFILE )
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${SPECFILE} ${LIBRARY_OUTPUT_PATH}/${SPECFILE} COPYONLY)
endfunction( install_spec )

if( NOT APPLE )

    function( install_lib TARGET )
        install( TARGETS ${TARGET}
                 RUNTIME DESTINATION ${INSTALL_LIBRARY_PATH}
                 LIBRARY DESTINATION ${INSTALL_LIBRARY_PATH} )
    endfunction( install_lib )

    function( install_bin TARGET )
        install( TARGETS ${TARGET} DESTINATION ${INSTALL_BINARY_PATH} )
    endfunction( install_bin )

    function( install_plugin TARGET )
        install( TARGETS ${TARGET}
                 RUNTIME DESTINATION ${INSTALL_PLUGIN_PATH}
                 LIBRARY DESTINATION ${INSTALL_PLUGIN_PATH} )
        install( FILES ${TARGET}.spec DESTINATION ${INSTALL_PLUGIN_PATH} )
        install_spec(${TARGET}.spec)
    endfunction( install_plugin )

else()

    function( install_lib TARGET )
    endfunction( install_lib )

    function( install_bin TARGET )
    endfunction( install_bin )

    function( install_plugin TARGET )
        install( FILES ${TARGET}.spec DESTINATION ${INSTALL_PLUGIN_PATH} )
        install_spec(${TARGET}.spec)
    endfunction( install_plugin )
endif()

if( APPLE )

#
# This is evil! I don't know why it works this way.
# But it works.
#
# First - collect all Qt and app plugins. Then call that bloody
# cmake FIXUP_BUNDLE macro which copis all files into MacOS directory.
# So it's moved back to plugins tree where its expected by a) Qt b) app
#
# I hate it.
#

install(CODE "

# Dunno what id this var for...
SET(DIRS ${QT_LIBRARY_DIRS};${QT_PLUGINS_DIR};${APP_PLUGIN_PATH})

# qt_menu.nib is mandatory for mac
if (${QT_USE_FRAMEWORKS} STREQUAL \"ON\")
    file(COPY ${QT_LIBRARY_DIR}/QtGui.framework/Resources/qt_menu.nib
         DESTINATION ${APP_DATA_PATH})
else ()
    file(COPY ${QT_LIBRARY_DIR}/Resources/qt_menu.nib
         DESTINATION ${APP_DATA_PATH})
endif ()

# the qt.conf is mandatory too
file(WRITE ${APP_DATA_PATH}/qt.conf \"[Paths]
Plugins = PlugIns
\")

# copy all (required) Qt plugs into bundle
message(STATUS \"Searching for Qt plugs in: ${QT_PLUGINS_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file( COPY ${QT_PLUGINS_DIR}/
      DESTINATION ${APP_APPLICATION_PATH}/Andromeda.app/Contents/PlugIns/
      REGEX \"(designer|script|sqlite|mng|phonon|debug)\" EXCLUDE )

# try to FIXUP_BUNDLE - to change otool -L paths
# currently it creates lots of false warnings when plugins are compiled as dylibs
# warning: cannot resolve item 'libtepsonic_lastfmscrobbler.dylib'
# etc. Ignore it.

include(BundleUtilities)

message( STATUS \"Searching for plugins in bundle: ${APP_APPLICATION_PATH}/Andromeda.app/Contents/PlugIns/*${CMAKE_SHARED_LIBRARY_SUFFIX}\" )
file( GLOB_RECURSE inplugs ${APP_APPLICATION_PATH}/Andromeda.app/Contents/PlugIns/*${CMAKE_SHARED_LIBRARY_SUFFIX} )

fixup_bundle(${APP_APPLICATION_PATH}/Andromeda.app \"\${inplugs}\" ${APP_APPLICATION_PATH}/Andromeda.app/Contents/PlugIns/ )

# Now we fixup plugins' id. Not really needed, but id should be @executable_path/../PlugIns/libName.dylib
# instead of @executable_path/../MacOs/libName.dylib for plugins
foreach(library \${inplugs})
    get_filename_component(name \${library} NAME)
    get_filename_component(path \${library} PATH)
    get_filename_component(dir \${path} NAME)
    execute_process( COMMAND install_name_tool -id @executable_path/../PlugIns/\${dir}/\${name} \${library} )
endforeach()

file( COPY ${APP_APPLICATION_PATH}/Andromeda.app DESTINATION ${INSTALL_PATH} )
file( REMOVE ${PROJECT_BINARY_DIR}/Andromeda.dmg )
execute_process( COMMAND hdiutil create -format UDZO -srcfolder ${INSTALL_PATH} ${PROJECT_BINARY_DIR}/Andromeda.dmg )
")

elseif(WIN32)

    if( MINGW )
        install( FILES ${QT_BINARY_DIR}/mingwm10${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${INSTALL_LIBRARY_PATH} )
        install( FILES ${QT_BINARY_DIR}/libgcc_s_dw2-1${CMAKE_SHARED_LIBRARY_SUFFIX} DESTINATION ${INSTALL_LIBRARY_PATH} )
    endif()

    macro(deploy_qt_library name)
        install( FILES "${QT_BINARY_DIR}/${name}4${CMAKE_SHARED_LIBRARY_SUFFIX}" DESTINATION ${INSTALL_LIBRARY_PATH} )
    endmacro()

    macro(deploy_qt_libraries)
        foreach(library ${ARGN})
                deploy_qt_library(${library})
        endforeach()
    endmacro()

    list(APPEND QT_LIBS
        QtCore
        QtGui
        QtNetwork
        QtWebKit
        QtXmlPatterns
        phonon
    )

    deploy_qt_libraries( ${QT_LIBS} )

    install(CODE "
        file( COPY ${QT_PLUGINS_DIR}/
              DESTINATION ${INSTALL_PATH}/plugins/
              REGEX \"(designer|script|sqlite|mng|phonon|d4\\\\.|\\\\.lib|\\\\.a)\" EXCLUDE )
    ")

elseif( UNIX )

    set( CMAKE_INSTALL_RPATH_USE_LINK_PATH true )
    set( CMAKE_INSTALL_RPATH ${INSTALL_LIBRARY_PATH} )

endif()

get_filename_component(INSTALL_TRANSLATIONS_PATH ${INSTALL_TRANSLATIONS_PATH} PATH)
install( DIRECTORY ${APP_TRANSLATIONS_PATH} DESTINATION ${INSTALL_TRANSLATIONS_PATH} )
