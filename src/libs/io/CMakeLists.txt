set( GLOBAL_HEADERS
    IO
    QDriveInfo
    QDriveController
   )
set( HEADERS
    io_global.h
    qdrivecontroller.h
    qdrivecontroller_p.h
    qdriveinfo.h
    qdriveinfo_p.h
    qsystemerror_p.h
   )
set( SOURCES
    qdrivecontroller.cpp
    qdriveinfo.cpp
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
    userenv Netapi32 Mpr
   )

elseif( APPLE )

set( HEADERS
    ${HEADERS}
    qdrivecontroller_mac_p.h
   )

set( SOURCES
    ${SOURCES}
    qdrivecontroller_mac.cpp
    qdriveinfo_mac.cpp
   )

set( LIBS
    "-framework CoreServices" "-framework DiskArbitration" "-framework IOKit"
   )

elseif( UNIX )

set( HEADERS
    ${HEADERS}
    qdrivecontroller_linux_p.h
   )

set( SOURCES
    ${SOURCES}
    qdrivecontroller_linux.cpp
    qdriveinfo_linux.cpp
   )

endif()

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtfilecopier )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/include )

add_definitions( -DIO_LIBRARY )

add_library( io SHARED ${GLOBAL_HEADERS} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( io ${QT_LIBRARIES} ${LIBS} )

