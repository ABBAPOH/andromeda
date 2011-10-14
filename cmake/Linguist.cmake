file( MAKE_DIRECTORY ${QM_OUTPUT_PATH} )

macro( qt4_wrap_ts outfiles )

    set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${QM_OUTPUT_PATH})
    qt4_add_translation( ${outfiles} ${TS_FILES} )

endmacro( qt4_wrap_ts )

macro(copy_qt_qm)

    set( QT_QM_FILES qt_ru.qm qt_cs.qm)
    foreach( QM_FILE ${QT_QM_FILES} )
        configure_file(  ${QT_TRANSLATIONS_DIR}/${QM_FILE} ${QM_OUTPUT_PATH} COPYONLY)
    endforeach( QM_FILE )

endmacro(copy_qt_qm)

copy_qt_qm()
