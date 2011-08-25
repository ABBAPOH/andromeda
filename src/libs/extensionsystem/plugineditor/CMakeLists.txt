set( HEADERS
    mainwindow.h
   )
set( SOURCES
    main.cpp
    mainwindow.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( EDMOC_SOURCES ${HEADERS} )
qt4_wrap_ui( EDUI_HEADERS ${FORMS} )

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DEXTENSIONSYSTEM_LIBRARY )

add_executable( plugineditor WIN32 ${HEADERS} ${SOURCES} ${EDMOC_SOURCES} ${EDUI_HEADERS} )
target_link_libraries( plugineditor ${QT_LIBRARIES} extensionsystem )

