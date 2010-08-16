set( HEADERS 
    qtlockedfile.h
   )
if( WIN32 )
set( SOURCES 
    qtlockedfile.cpp
    qtlockedfile_win.cpp
   )
else( WIN32 )
set( SOURCES 
    qtlockedfile.cpp
    qtlockedfile_unix.cpp
   )
endif( WIN32 )

add_definitions( -DQT_QTLOCKEDFILE_EXPORT )

add_library( qtlockedfile STATIC ${HEADERS} ${SOURCES} )
target_link_libraries( qtlockedfile ${QT_LIBRARIES} )

