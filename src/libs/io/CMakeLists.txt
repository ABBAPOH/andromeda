set( HEADERS
    IO
    io_global.h
   )
set( SOURCES
    main.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/qtfilecopier )
include_directories( ${PROJECT_SOURCE_DIR}/src/shared/include )

add_definitions( -DIO_LIBRARY )
#add_definitions( -DQT_QTCOPYDIALOG_IMPORT )

add_library( io SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( io ${QT_LIBRARIES} qtfilecopier )

