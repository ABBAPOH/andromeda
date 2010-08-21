qt4_wrap_single_cpp( registrationsystem_test.cpp )

include_directories( ${PROJECT_SOURCE_DIR}/src/libs/registrationsystem )

add_executable( registrationsystem_test registrationsystem_test.cpp )
target_link_libraries( registrationsystem_test ${QT_LIBRARIES} registrationsystem )

add_test( NAME RegistrationSystem_Test COMMAND registrationsystem_test )
