if (APPLE)
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
if (${QT_USE_FRAMEWORKS})
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
file(COPY ${QT_PLUGINS_DIR}/
        DESTINATION ${APP_APPLICATION_PATH}/Andromeda.app/Contents/PlugIns/ 
        REGEX \"(designer|script|debug|sqlite|ico|gif|tiff|mng|phonon)\" EXCLUDE)

# try to FIXUP_BUNDLE - to change otool -L paths
# currently it creates lots of false warnings when plugins are compiled as dylibs
# warning: cannot resolve item 'libtepsonic_lastfmscrobbler.dylib'
# etc. Ignore it.
message(STATUS \"Searching for plugs in bundle: ${APP_APPLICATION_PATH}/Andromeda.app/Contents/PlugIns/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
file(GLOB_RECURSE inplugs ${APP_APPLICATION_PATH}/Andromeda.app/Contents/PlugIns/*${CMAKE_SHARED_LIBRARY_SUFFIX})
message(WARNING \"inplugs: \${inplugs}\")
message(STATUS \"Serching for Andromeda libs\")
file(GLOB andromeda_plugs ${APP_LIBRARY_PATH}/*${CMAKE_SHARED_LIBRARY_SUFFIX})


include(BundleUtilities)
fixup_bundle(${APP_APPLICATION_PATH}/Andromeda.app \"\${inplugs};\${andromeda_plugs}\" ${APP_APPLICATION_PATH}/Andromeda.app/Contents/PlugIns/ )

")
    
endif (APPLE)

