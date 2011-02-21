set( GLOBAL_HEADER CorePlugin editormanager/EditorManager)
set( HEADERS
    coreplugin_global.h
    core.h
    core_p.h
    coreplugin.h
    ifile.h
    editormanager/editormanager.h
    editormanager/ieditor.h
    editormanager/ieditorfactory.h
    editormanager/editorview.h
    editormanager/editorhistory.h
    editormanager/historyitem.h
   )
set( SOURCES
    core.cpp
    coreplugin.cpp
    ifile.cpp
    editormanager/editormanager.cpp
    editormanager/ieditor.cpp
    editormanager/ieditorfactory.cpp
    editormanager/editorview.cpp
    editormanager/editorhistory.cpp
    editormanager/historyitem.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( editormanager )

add_definitions( -DCOREPLUGIN_LIBRARY )

add_library( coreplugin SHARED ${GLOBAL_HEADER} ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( coreplugin ${QT_LIBRARIES} extensionsystem )

install_spec( coreplugin.spec )
