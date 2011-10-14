macro( qt4_wrap_ts outfiles )

    set_source_files_properties(${TS_FILES} PROPERTIES OUTPUT_LOCATION ${QM_OUTPUT_PATH})
    qt4_add_translation( ${outfiles} ${TS_FILES} )

endmacro( qt4_wrap_ts )
