set( HEADERS
   )
set( SOURCES
    main.cpp
   )

#qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )

add_executable( app ${SOURCES} )

target_link_libraries( app ${QT_LIBRARIES} extensionsystem )

