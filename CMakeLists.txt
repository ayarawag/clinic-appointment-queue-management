cmake_minimum_required(VERSION 3.10)
project(ClinicApp CXX)

# لغة C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ----------------------------------------------------
# اكتشاف المكتبات الضرورية (SQLite3 و Threads)
# ----------------------------------------------------
find_package(SQLite3 REQUIRED)
find_package(Threads REQUIRED) 

# تعريف المسارات
set(SRC_DIR "${CMAKE_SOURCE_DIR}/models")
set(DB_DIR "${CMAKE_SOURCE_DIR}/database")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/tests")
set(GTEST_DIR "${CMAKE_SOURCE_DIR}/libs/googletest-1.15.0")
# مسار مجلد الدوال الجديدة
set(FEATURES_DIR "${CMAKE_SOURCE_DIR}/features") 

# ----------------------------------------------------
# ملفات المشروع الرئيسية
# ----------------------------------------------------
set(SRC_FILES
    ${SRC_DIR}/patient.cpp
    ${SRC_DIR}/doctor.cpp
    ${SRC_DIR}/appointment.cpp
    ${SRC_DIR}/queue.cpp
    # ملف النوتيفيكيشن (مفقود في القائمة السابقة)
    ${SRC_DIR}/notification.cpp

    # جميع الدوال الموجودة في مجلد features (بناءً على الصورة المرفوعة)
    ${FEATURES_DIR}/booking.cpp
    ${FEATURES_DIR}/cancel_appointment.cpp
    ${FEATURES_DIR}/doctor_schedule.cpp
    ${FEATURES_DIR}/login.cpp
    ${FEATURES_DIR}/notification.cpp
    ${FEATURES_DIR}/payment.cpp
    ${FEATURES_DIR}/queue_managment.cpp
    ${FEATURES_DIR}/register.cpp
    ${FEATURES_DIR}/reschedule.cpp
    ${FEATURES_DIR}/viewAppointments.cpp

    ${DB_DIR}/db_connection.cpp
    ${DB_DIR}/sqlite3.c
    ${CMAKE_SOURCE_DIR}/main.cpp
)

# ملفات الاختبارات (تم إضافة الملفات المفقودة)
set(TEST_FILES
    ${TEST_DIR}/gtest_main.cpp
    ${TEST_DIR}/test_basic.cpp
    ${TEST_DIR}/test_patient.cpp
    ${TEST_DIR}/test_doctor.cpp
    ${TEST_DIR}/test_appointment.cpp
    ${TEST_DIR}/test_queue.cpp
    ${TEST_DIR}/test_notification.cpp
)

# إضافة GTest
add_subdirectory(${GTEST_DIR} ${CMAKE_BINARY_DIR}/googletest-build)

# ----------------------------------------------------
# هدف المشروع الرئيسي (clinic)
# ----------------------------------------------------
add_executable(clinic ${SRC_FILES})
target_include_directories(clinic PRIVATE ${SRC_DIR} ${DB_DIR} ${FEATURES_DIR}) # تضمين مجلد features
# ربط التطبيق بمكتبة SQLite3
target_link_libraries(clinic PRIVATE SQLite3::SQLite3)

# ----------------------------------------------------
# هدف اختبارات GoogleTest (run_tests)
# ----------------------------------------------------
add_executable(run_tests ${SRC_FILES} ${TEST_FILES})
target_include_directories(run_tests PRIVATE 
    ${SRC_DIR} 
    ${DB_DIR} 
    ${FEATURES_DIR}
    ${GTEST_DIR}/googletest/include
    ${GTEST_DIR}/googlemock/include
)

# ربط هدف الاختبارات: GTest، SQLite3، و Threads
target_link_libraries(run_tests 
    PRIVATE 
    gtest_main 
    gmock_main 
    SQLite3::SQLite3
    Threads::Threads 
)