set( HEADERS
    editormanager.h
    editormanager_p.h
    ieditor.h
    ieditorfactory.h
    editorview.h
    editorhistory.h
    historyitem.h
   )
set( SOURCES
    editormanager.cpp
    ieditor.cpp
    ieditorfactory.cpp
    editorview.cpp
    editorhistory.cpp
    historyitem.cpp
   )
set( FORMS
   )

qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
qt4_wrap_ui( UI_HEADERS ${FORMS} )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/extensionsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/libs/registrationsystem )
include_directories( ${PROJECT_SOURCE_DIR}/src/plugins/coreplugin )

add_definitions( -DCOREPLUGIN_LIBRARY )

add_library( editormanager STATIC ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} )
target_link_libraries( editormanager ${QT_LIBRARIES} extensionsystem registrationsystem )

