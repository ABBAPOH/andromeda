function( qt4_wrap_single_cpp SOURCE )
    get_filename_component( SOURCENAME ${SOURCE} NAME_WE )
    qt4_generate_moc( ${SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/${SOURCENAME}.moc )
    set_source_files_properties( ${SOURCE}  OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${SOURCENAME}.moc )
endfunction( qt4_wrap_single_cpp )

set( GLOBAL_HEADERS
    IO
    QDriveInfo
    QDriveController
    QFileCopier
   )
set( HEADERS
    inqt5/qstandardpaths.h
    io_global.h
    info2_p.h
    directorydetails.h
    fileimageprovider.h
    kdesettings.h
    qdrivecontroller.h
    qdrivecontroller_p.h
    qdriveinfo.h
    qdriveinfo_p.h
    qfilecopier.h
    qfilecopier_p.h
    qmimedatabase.h
    qmimedatabase_p.h
    qmimeglobpattern_p.h
    qmimemagicrule_p.h
    qmimemagicrulematcher_p.h
    qmimeprovider_p.h
    qmimetype.h
    qmimetype_p.h
    qmimetypeparser_p.h
    qsystemerror_p.h
    qtrash.h
    qtrash_p.h
    qtrashfileinfo.h
    qtrashfileinfo_p.h
   )
set( SOURCES
    inqt5/qstandardpaths.cpp
    info2.cpp
    directorydetails.cpp
    fileimageprovider.cpp
    kdesettings.cpp
    qdrivecontroller.cpp
    qdriveinfo.cpp
    qfilecopier.cpp
    qmimedatabase.cpp
    qmimeglobpattern.cpp
    qmimemagicrule.cpp
    qmimemagicrulematcher.cpp
    qmimeprovider.cpp
    qmimetype.cpp
    qmimetypeparser.cpp
    qsystemerror.cpp
    qtrash.cpp
    qtrashfileinfo.cpp
   )
set( FORMS
   )
set( RESOURCES resources.qrc )

if( WIN32 )

set( SOURCES
    ${SOURCES}
    inqt5/qstandardpaths_win.cpp
    qdrivecontroller_win.cpp
    qdriveinfo_win.cpp
    qtrash_win.cpp
   )

set( LIBS
    userenv Netapi32 Mpr user32 Winmm
   )

qt4_wrap_single_cpp( qdrivecontroller_win.cpp )

elseif( APPLE )

set( SOURCES
    ${SOURCES}
    inqt5/qstandardpaths_mac.cpp
    qdrivecontroller_mac.cpp
    qdriveinfo_mac.cpp
    qtrash_mac.mm
   )

set( LIBS
    "-framework CoreServices" "-framework DiskArbitration" "-framework IOKit" "-framework QuickLook" "-framework AppKit"
   )

qt4_wrap_single_cpp( qdrivecontroller_mac.cpp )

elseif( UNIX )

set( HEADERS
    ${HEADERS}
    qxdg.h
)

set( SOURCES
    ${SOURCES}
    inqt5/qstandardpaths_unix.cpp
    qdrivecontroller_linux.cpp
    qdriveinfo_linux.cpp
    qtrash_linux.cpp
    qxdg.cpp
   )

qt4_wrap_single_cpp( qdrivecontroller_linux.cpp )

set( QT_USE_QTDBUS TRUE )
include( ${QT_USE_FILE} )

endif()

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )
qt4_add_resources( QRC_SOURCES ${RESOURCES} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/inqt5 )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/include )

add_definitions( -DIO_LIBRARY )
add_definitions( -DQT_QTCOPYDIALOG_EXPORT )
add_definitions( -DQDRIVEINFO_DLL )
if( MSVC )
    # fucken microsoft compiler!!!
    add_definitions( /Zc:wchar_t- )
endif( MSVC )

add_library( io SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} )
target_link_libraries( io ${QT_LIBRARIES} ${LIBS} )

install_lib( io )
