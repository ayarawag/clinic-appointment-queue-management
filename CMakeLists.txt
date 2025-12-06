cmake_minimum_required(VERSION 3.10)
project(ClinicApp)

# لغة C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. البحث عن مكتبة Threads
find_package(Threads REQUIRED)

# تعريف المسارات
set(SRC_DIR "${CMAKE_SOURCE_DIR}/models")
set(FEATURES_DIR "${CMAKE_SOURCE_DIR}/features") # مسار الـ features
set(DB_DIR "${CMAKE_SOURCE_DIR}/database")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/tests")
set(GTEST_DIR "${CMAKE_SOURCE_DIR}/libs/googletest-1.15.0")

#  تحديث هام: إضافة كل ملفات features/ 
# مسح شامل لجميع ملفات .cpp في مجلد features
file(GLOB FEATURE_FILES "${FEATURES_DIR}/*.cpp")

# 2. ملفات المشروع الرئيسية (إضافة ملفات Features)
set(SRC_FILES
    ${SRC_DIR}/patient.cpp
    ${SRC_DIR}/doctor.cpp
    ${SRC_DIR}/appointment.cpp
    ${SRC_DIR}/queue.cpp
    ${SRC_DIR}/notification.cpp        # ملف تنفيذ models/notification.cpp
    ${DB_DIR}/db_connection.cpp
    ${DB_DIR}/sqlite3.c
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${FEATURE_FILES}                  # إضافة جميع ملفات features/
)

# 3. ملفات الاختبارات (إضافة ملف اختبار الإشعارات)
set(TEST_FILES
    ${TEST_DIR}/gtest_main.cpp
    ${TEST_DIR}/test_basic.cpp
    ${TEST_DIR}/test_patient.cpp
    ${TEST_DIR}/test_notification.cpp # ملف اختبار الإشعارات
)

# إضافة GTest
add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/googletest-build)

# هدف المشروع الرئيسي (التطبيق العادي)
add_executable(clinic ${SRC_FILES})
target_include_directories(clinic PRIVATE ${SRC_DIR} ${DB_DIR} ${FEATURES_DIR}) # تضمين features
target_link_libraries(clinic sqlite3)


# هدف اختبارات GoogleTest
add_executable(run_tests ${SRC_FILES} ${TEST_FILES})
target_include_directories(run_tests PRIVATE 
    ${SRC_DIR} 
    ${DB_DIR} 
    ${FEATURES_DIR} # تضمين features
    ${GTEST_DIR}/googletest/include
)

# 4. الربط بالمكتبات المطلوبة 
target_link_libraries(run_tests 
    gtest_main 
    gmock_main 
    Threads::Threads 
    sqlite3
)

# تفعيل تغطية الكود
if(ENABLE_COVERAGE)
    message(STATUS "Code Coverage Enabled.")
    target_compile_options(run_tests PRIVATE -ftest-coverage -fprofile-arcs)
    target_link_options(run_tests PRIVATE -ftest-coverage -fprofile-arcs)

    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)

    if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        add_custom_target(coverage_clean
            COMMAND find ${CMAKE_BINARY_DIR} -name "*.gcda" -exec rm -f {} +
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

        add_custom_target(coverage
            COMMAND ${LCOV_EXECUTABLE} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
            COMMAND ${LCOV_EXECUTABLE} --remove coverage.info '*/googletest/*' '*/sqlite3.c' '*_autogen/*' --output-file coverage.info.cleaned
            COMMAND ${GENHTML_EXECUTABLE} coverage.info.cleaned --output-directory ${CMAKE_BINARY_DIR}/coverage-html
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            DEPENDS run_tests
        )
    else()
        message(WARNING "lcov or genhtml not found. Coverage targets disabled.")
    endif()
endif()