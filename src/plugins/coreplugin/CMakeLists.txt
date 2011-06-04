set( GLOBAL_HEADER CorePlugin )
set( HEADERS
    coreplugin_global.h
    core.h
    core_p.h
    coreplugin.h
    globalhistory.h
    history.h
    historyitem.h
    ifile.h
    ihistoryview.h
    imainview.h
    page.h
    perspectivemanager.h
   )
set( SOURCES
    core.cpp
    coreplugin.cpp
    globalhistory.cpp
    history.cpp
    historyitem.cpp
    ifile.cpp
    ihistoryview.cpp
    imainview.cpp
    page.cpp
    perspectivemanager.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( editormanager )

add_definitions( -DCOREPLUGIN_LIBRARY )

add_library( coreplugin SHARED ${GLOBAL_HEADER} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( coreplugin ${QT_LIBRARIES} extensionsystem guisystem )

install_spec( coreplugin.spec )
