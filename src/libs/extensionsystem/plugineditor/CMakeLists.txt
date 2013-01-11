set( HEADERS
    mainwindow.h
   )
set( SOURCES
    main.cpp
    mainwindow.cpp
   )
set( FORMS
   )

set( MOC_SOURCES )
set( UI_HEADERS )
qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#add_definitions( -DEXTENSIONSYSTEM_LIBRARY )

add_executable( plugineditor WIN32 ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( plugineditor ${QT_LIBRARIES} libextensionsystem )

if( MSVC )
    set_target_properties( plugineditor PROPERTIES LINK_FLAGS /ENTRY:"mainCRTStartup" )
endif()

