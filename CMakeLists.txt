cmake_minimum_required(VERSION 3.10)
project(clinic)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# حل ABI
add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=0)

# Include dirs
include_directories(
    .
    ${PROJECT_SOURCE_DIR}/models
    ${PROJECT_SOURCE_DIR}/features
    ${PROJECT_SOURCE_DIR}/database
    ${PROJECT_SOURCE_DIR}/tests
    ${PROJECT_SOURCE_DIR}/libs/googletest-1.15.0/googletest/include
)

# ----------------------------
# GoogleTest موجود محلياً
# ----------------------------
add_subdirectory(libs/googletest-1.15.0)

# ----------------------------
# Source files
# ----------------------------
set(SOURCE_FILES
    main.cpp

    models/patient.cpp
    models/doctor.cpp
    models/appointment.cpp
    models/queue.cpp
    models/notification.cpp

    features/register.cpp
    features/login.cpp
    features/booking.cpp
    features/cancel_reschedule.cpp
    features/payment.cpp
    features/doctor_schedule.cpp
    features/queue_managment.cpp
    features/ViewAppointments.cpp
    features/notification.cpp

    database/db_connection.cpp
    database/sqlite3.c

    # Utils (المفروض كانوا ناقصين)
    utils/password_utils.cpp
    utils/time_utils.cpp
)

add_executable(clinic ${SOURCE_FILES})

# ----------------------------
# TEST EXECUTABLE
# ----------------------------
add_executable(run_tests
    tests/gtest_main.cpp
    tests/test_basic.cpp
    tests/test_patient.cpp
    tests/test_doctor.cpp
    tests/test_appointment.cpp
    tests/test_queue.cpp
    tests/test_register.cpp
    tests/test_notification.cpp
)

target_link_libraries(run_tests gtest gtest_main pthread sqlite3)