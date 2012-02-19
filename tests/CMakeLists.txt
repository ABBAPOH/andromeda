if( NOT DISABLE_TESTS )
    enable_testing()

    set( CTEST_ARGUMENTS --output-on-failure )
    add_custom_target( test-verbose )
    add_custom_command( TARGET test-verbose POST_BUILD COMMAND ${CMAKE_CTEST_COMMAND} ${CTEST_ARGUMENTS} )

add_subdirectory( plugins )
add_subdirectory( manual )
add_subdirectory( auto )

endif()
