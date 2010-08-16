set( HEADERS
    qtlocalpeer.h
    qtsingleapplication.h
   )
set( SOURCES
    qtlocalpeer.cpp
    qtsingleapplication.cpp
   )
set( QT_USE_QTNETWORK true )
INCLUDE(${QT_USE_FILE})
qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

include_directories( ../qtlockedfile )

#add_definitions( -DQT_QTLOCKEDFILE_IMPORT )
add_definitions( -DQT_QTSINGLEAPPLICATION_EXPORT )

add_library( qtsingleapplication STATIC ${HEADERS} ${SOURCES} ${MOC_SOURCES} )
target_link_libraries( qtsingleapplication ${QT_LIBRARIES} qtlockedfile  )
