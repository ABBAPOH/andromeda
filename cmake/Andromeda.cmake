include("cmake/LibSuffix.cmake")

# build structure
if( APPLE )
    set( APP_APPLICATION_PATH  ${PROJECT_BINARY_DIR}/bin )
    set( APP_BINARY_PATH       ${APP_APPLICATION_PATH}/Andromeda.app/Contents/MacOs )
    set( APP_LIBRARY_PATH      ${APP_APPLICATION_PATH}/Andromeda.app/Contents/MacOs )
    set( APP_PLUGIN_PATH       ${APP_APPLICATION_PATH}/Andromeda.app/Contents/PlugIns/andromeda )
    set( APP_DATA_PATH         ${APP_APPLICATION_PATH}/Andromeda.app/Contents/Resources )
    set( APP_TRANSLATIONS_PATH ${APP_APPLICATION_PATH}/Andromeda.app/Contents/Resources/translations )

    set( INSTALL_PATH              ${PROJECT_BINARY_DIR}/Andromeda )
    set( INSTALL_BINARY_PATH       ${INSTALL_PATH} )
    set( INSTALL_LIBRARY_PATH      ${INSTALL_PATH}/Andromeda.app/Contents/Frameworks )
    set( INSTALL_PLUGIN_PATH       ${INSTALL_PATH}/Andromeda.app/Contents/PlugIns/andromeda )
    set( INSTALL_DATA_PATH         ${INSTALL_PATH}/Andromeda.app/Contents/Resources )
    set( INSTALL_TRANSLATIONS_PATH ${INSTALL_PATH}/Andromeda.app/Contents/Resources/translations )
elseif( WIN32 )
    set( APP_APPLICATION_PATH  ${PROJECT_BINARY_DIR} )
    set( APP_BINARY_PATH       ${APP_APPLICATION_PATH}/bin )
    set( APP_LIBRARY_PATH      ${APP_APPLICATION_PATH}/bin )
    set( APP_PLUGIN_PATH       ${APP_APPLICATION_PATH}/plugins/andromeda )
    set( APP_DATA_PATH         ${APP_APPLICATION_PATH}/resources )
    set( APP_TRANSLATIONS_PATH ${APP_APPLICATION_PATH}/translations )

    set( INSTALL_PATH              ${PROJECT_BINARY_DIR}/andromeda )
    set( INSTALL_BINARY_PATH       ${INSTALL_PATH}/bin )
    set( INSTALL_LIBRARY_PATH      ${INSTALL_PATH}/bin )
    set( INSTALL_PLUGIN_PATH       ${INSTALL_PATH}/plugins/andromeda )
    set( INSTALL_DATA_PATH         ${INSTALL_PATH}/resources )
    set( INSTALL_TRANSLATIONS_PATH ${INSTALL_PATH}/translations )
elseif( UNIX )
    set( APP_APPLICATION_PATH  ${PROJECT_BINARY_DIR} )
    set( APP_BINARY_PATH       ${APP_APPLICATION_PATH}/bin )
    set( APP_LIBRARY_PATH      ${APP_APPLICATION_PATH}/lib${LIB_SUFFIX} )
    set( APP_PLUGIN_PATH       ${APP_APPLICATION_PATH}/lib${LIB_SUFFIX}/andromeda/plugins/andromeda )
    set( APP_DATA_PATH         ${APP_APPLICATION_PATH}/share/andromeda )
    set( APP_TRANSLATIONS_PATH ${APP_APPLICATION_PATH}/share/andromeda/translations )

    set( INSTALL_PATH              ${CMAKE_INSTALL_PREFIX} )
    set( INSTALL_BINARY_PATH       ${INSTALL_PATH}/bin )
    set( INSTALL_LIBRARY_PATH      ${INSTALL_PATH}/lib${LIB_SUFFIX}/andromeda )
    set( INSTALL_PLUGIN_PATH       ${INSTALL_PATH}/lib${LIB_SUFFIX}/andromeda/plugins/andromeda )
    set( INSTALL_DATA_PATH         ${INSTALL_PATH}/share/andromeda )
    set( INSTALL_TRANSLATIONS_PATH ${INSTALL_PATH}/share/andromeda/translations )
endif()

file( MAKE_DIRECTORY ${APP_APPLICATION_PATH} )
file( MAKE_DIRECTORY ${APP_BINARY_PATH} )
file( MAKE_DIRECTORY ${APP_LIBRARY_PATH} )
file( MAKE_DIRECTORY ${APP_PLUGIN_PATH} )
file( MAKE_DIRECTORY ${APP_DATA_PATH} )
file( MAKE_DIRECTORY ${APP_TRANSLATIONS_PATH} )

# option to make libraries have same name on different platforms
set( CMAKE_SHARED_LIBRARY_PREFIX "lib" )

# translation helpers
macro( qt4_wrap_ts outfiles )
    set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${APP_TRANSLATIONS_PATH} )
    qt4_add_translation( ${outfiles} ${TS_FILES} )
endmacro( qt4_wrap_ts )

macro(copy_qt_qm)
    set( QT_QM_FILES qt_ru.qm qt_cs.qm)
    foreach( QM_FILE ${QT_QM_FILES} )
        if( EXISTS ${QT_TRANSLATIONS_DIR}/${QM_FILE} )
            configure_file( ${QT_TRANSLATIONS_DIR}/${QM_FILE} ${APP_TRANSLATIONS_PATH} COPYONLY )
        endif()
    endforeach( QM_FILE )
endmacro(copy_qt_qm)
copy_qt_qm()

# compiler options
if( NOT MSVC )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -pedantic" )
endif( NOT MSVC )
