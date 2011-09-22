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
    io_global.h
    qdrivecontroller.h
    qdrivecontroller_p.h
    qdriveinfo.h
    qdriveinfo_p.h
    qfilecopier.h
    qfilecopier_p.h
    qsystemerror_p.h
   )
set( SOURCES
    qdrivecontroller.cpp
    qdriveinfo.cpp
    qfilecopier.cpp
    qsystemerror.cpp
   )
set( FORMS
   )

if( WIN32 )

set( SOURCES
    ${SOURCES}
    qdrivecontroller_win.cpp
    qdriveinfo_win.cpp
   )

set( LIBS
    userenv Netapi32 Mpr user32 Winmm
   )

qt4_wrap_single_cpp( qdrivecontroller_win.cpp )

elseif( APPLE )

set( SOURCES
    ${SOURCES}
    qdrivecontroller_mac.cpp
    qdriveinfo_mac.cpp
   )

set( LIBS
    "-framework CoreServices" "-framework DiskArbitration" "-framework IOKit"
   )

qt4_wrap_single_cpp( qdrivecontroller_mac.cpp )

elseif( UNIX )

set( SOURCES
    ${SOURCES}
    qdrivecontroller_linux.cpp
    qdriveinfo_linux.cpp
   )

qt4_wrap_single_cpp( qdrivecontroller_linux.cpp )

endif()

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/include )

add_definitions( -DIO_LIBRARY )
add_definitions( -DQT_QTCOPYDIALOG_EXPORT )
add_definitions( -DQDRIVEINFO_DLL )
if( MSVC )
    # fucken microsoft compiler!!!
    add_definitions( /Zc:wchar_t- )
endif( MSVC )

add_library( io SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( io ${QT_LIBRARIES} ${LIBS} )

install_lib( io )
