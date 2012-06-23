find_package( PopplerQt4 0.14 QUIET PATHS ${CMAKE_CURRENT_SOURCE_DIR} ) # This is part of the kdelibs development package; the included PopplerQt4Config*.cmake files should also work
if( POPPLER_QT4_FOUND )
    add_definitions( ${QT_DEFINITIONS} ${POPPLER_QT4_DEFINITIONS} )
    include_directories( ${POPPLER_QT4_INCLUDE_DIR} )

    set( HEADERS
        utils/bookmarkshandler.h
        utils/icon.h
        utils/filesettings.h
        utils/globallocale.h
        utils/lineedit.h
        utils/selectaction.h
        utils/selectpageaction.h
        utils/zoomaction.h
        findwidget.h
        pageitem.h
        pdfview.h
        pdfview_p.h
        pdfview_global.h
        pdfvieweditor.h
        pdfviewplugin.h
    )
    set( SOURCES
        utils/bookmarkshandler.cpp
        utils/filesettings.cpp
        utils/globallocale.cpp
        utils/lineedit.cpp
        utils/selectaction.cpp
        utils/selectpageaction.cpp
        utils/zoomaction.cpp
        synctex/synctex_parser.c
        synctex/synctex_parser_utils.c
        findwidget.cpp
        pageitem.cpp
        pdfview.cpp
        pdfvieweditor.cpp
        pdfviewplugin.cpp
    )
    set( FORMS findwidget.ui )
    set( RESOURCES resources.qrc )
    set( TS_FILES translations/pdfviewplugin_fr.ts )

    qt4_wrap_cpp( MOC_SOURCES ${HEADERS} )
    qt4_wrap_ui( UI_HEADERS ${FORMS} )
    qt4_add_resources( QRC_SOURCES ${RESOURCES} )
    qt4_wrap_ts( QM_FILES ${TS_FILES} )

    include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

    add_definitions( -DPDFVIEW_LIBRARY )

    add_library( pdfviewplugin SHARED ${HEADERS} ${SOURCES} ${MOC_SOURCES} ${UI_HEADERS} ${QRC_SOURCES} ${QM_FILES} )
    target_link_libraries( pdfviewplugin ${QT_LIBRARIES} ${POPPLER_QT4_LIBRARIES} extensionsystem guisystem )
    add_dependencies( pdfviewplugin plugineditor)
    install_plugin( pdfviewplugin )
else( POPPLER_QT4_FOUND )
    message( STATUS "Could NOT find poppler-qt4 version 0.16 or higher. The PDF View plugin will NOT be built." )
endif( POPPLER_QT4_FOUND )
