find_package( PopplerQt4 0.14 QUIET PATHS ${CMAKE_CURRENT_SOURCE_DIR} ) # This is part of the kdelibs development package; the included PopplerQt4Config*.cmake files should also work
if( POPPLER_QT4_FOUND )
    set(PDFVIEWLIB_PRINT_PROGRAM lp)
    add_definitions(-DPDFVIEWLIB_PRINT_PROGRAM=\"${PDFVIEWLIB_PRINT_PROGRAM}\")

    add_definitions( ${QT_DEFINITIONS} ${POPPLER_QT4_DEFINITIONS} )
    include_directories( ${POPPLER_QT4_INCLUDE_DIR} )

    set( HEADERS
        lib/utils/bookmarkshandler.h
        lib/utils/filesettings.h
        lib/utils/globallocale.h
        lib/utils/selectaction.h
        lib/utils/selectpageaction.h
        lib/utils/zoomaction.h
        lib/actionhandler.h
        lib/pageitem.h
        lib/pdfview.h
        lib/pdfview_p.h
        lib/printhandler.h
        lib/synctexhandler.h
        utils/icon.h
        utils/lineedit.h
        findwidget.h
        pdfview_global.h
        pdfvieweditor.h
        pdfviewplugin.h
    )
    set( SOURCES
        lib/utils/bookmarkshandler.cpp
        lib/utils/filesettings.cpp
        lib/utils/globallocale.cpp
        lib/utils/selectaction.cpp
        lib/utils/selectpageaction.cpp
        lib/utils/zoomaction.cpp
        lib/synctex/synctex_parser.c
        lib/synctex/synctex_parser_utils.c
        lib/actionhandler.cpp
        lib/pageitem.cpp
        lib/pdfview.cpp
        lib/printhandler.cpp
        lib/synctexhandler.cpp
        utils/lineedit.cpp
        findwidget.cpp
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
    message( STATUS "Could NOT find poppler-qt4 version 0.14 or higher. The PDF View plugin will NOT be built." )
endif( POPPLER_QT4_FOUND )
