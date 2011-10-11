macro( get_git_revision VAR)
exec_program(
    "git"
    ${PROJECT_SOURCE_DIR}
    ARGS "rev-parse -q HEAD"
    OUTPUT_VARIABLE ${VAR} )
endmacro( get_git_revision )

get_git_revision(GIT_REVISION)
set( PROJECT_VERSION "0.0.0.1" )
