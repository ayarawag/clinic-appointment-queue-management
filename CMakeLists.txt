cmake_minimum_required(VERSION 3.10)
project(clinic)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=0)

include_directories(
    .
    ${PROJECT_SOURCE_DIR}/models
    ${PROJECT_SOURCE_DIR}/features
    ${PROJECT_SOURCE_DIR}/database
    ${PROJECT_SOURCE_DIR}/utils
    ${PROJECT_SOURCE_DIR}/tests
    ${PROJECT_SOURCE_DIR}/libs/googletest-1.15.0/googletest/include
)

# GoogleTest
add_subdirectory(libs/googletest-1.15.0)

# -------------------------------------
# Project Source Files
# -------------------------------------
set(PROJECT_SOURCES
    models/patient.cpp
    models/doctor.cpp
    models/appointment.cpp
    models/queue.cpp
    models/notification.cpp

    database/db_connection.cpp
    database/sqlite3.c

    utils/password_utils.cpp
    utils/time_utils.cpp
)

# Application Executable
add_executable(clinic
    main.cpp
    ${PROJECT_SOURCES}
    features/register.cpp
    features/login.cpp
    features/booking.cpp
    features/cancel_reschedule.cpp
    features/payment.cpp
    features/doctor_schedule.cpp
    features/queue_managment.cpp
    features/ViewAppointments.cpp
    features/notification.cpp
)

# -------------------------------------
# TEST EXECUTABLE
# -------------------------------------

file(GLOB TEST_FILES "tests/test_*.cpp")

add_executable(run_tests
    ${TEST_FILES}
    ${PROJECT_SOURCES}     # <<<<<< MUST ADD ALL PROJECT FILES
)

target_link_libraries(run_tests
    gtest
    gtest_main
    pthread
    sqlite3
)