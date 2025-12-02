cmake_minimum_required(VERSION 3.10)
project(ClinicApp)

# لغة C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# تعريف المسارات
set(SRC_DIR "${CMAKE_SOURCE_DIR}/models")
set(DB_DIR "${CMAKE_SOURCE_DIR}/database")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/tests")
set(GTEST_DIR "${CMAKE_SOURCE_DIR}/libs/googletest-1.15.0")

# ملفات المشروع الرئيسية
set(SRC_FILES
    ${SRC_DIR}/patient.cpp
    ${SRC_DIR}/doctor.cpp
    ${SRC_DIR}/appointment.cpp
    ${SRC_DIR}/queue.cpp
    ${DB_DIR}/db_connection.cpp
    ${DB_DIR}/sqlite3.c
    ${CMAKE_SOURCE_DIR}/main.cpp
)

# ملفات الاختبارات
set(TEST_FILES
    ${TEST_DIR}/gtest_main.cpp
    ${TEST_DIR}/test_basic.cpp
    ${TEST_DIR}/test_patient.cpp
)

# إضافة GTest
add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/googletest-build)

# هدف المشروع الرئيسي (التطبيق العادي)
add_executable(clinic ${SRC_FILES})
target_include_directories(clinic PRIVATE ${SRC_DIR} ${DB_DIR})

# هدف اختبارات GoogleTest
add_executable(run_tests ${SRC_FILES} ${TEST_FILES})
target_include_directories(run_tests PRIVATE ${SRC_DIR} ${DB_DIR} ${GTEST_DIR}/googletest/include)
target_link_libraries(run_tests gtest_main gmock_main)

# على ويندوز، ما نربطوش pthread
if(UNIX)
    target_link_libraries(run_tests Threads::Threads)
endif()