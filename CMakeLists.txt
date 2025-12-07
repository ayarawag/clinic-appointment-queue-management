# تعيين الحد الأدنى لإصدار CMake المطلوب
cmake_minimum_required(VERSION 3.10)

# تعريف اسم المشروع
project(clinic)

# تمكين معايير C++ الحديثة (C++14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ******** الحل الحاسم لأخطاء الربط (Linker Errors) ********
# فرض استخدام CXX03 ABI لضمان توحيد توقيعات std::string بين جميع الملفات.
# قمنا بتغيير القيمة من 1 إلى 0. هذا يحل مشكلة 'undefined reference'
add_compile_options(-D_GLIBCXX_USE_CXX11_ABI=0) 
# **************************************************************

# تحديد المسارات لتضمين ملفات الترويسة (Headers)
include_directories(
    .
    ${PROJECT_SOURCE_DIR}/models
    ${PROJECT_SOURCE_DIR}/features
    ${PROJECT_SOURCE_DIR}/database
    ${PROJECT_SOURCE_DIR}/tests
)

# تعريف متغير يحتوي على جميع الملفات المصدر (C++ و C)
set(SOURCE_FILES
    # الملف الرئيسي
    main.cpp

    # ملفات Models
    models/patient.cpp
    models/doctor.cpp
    models/appointment.cpp
    models/queue.cpp
    models/notification.cpp
    
    # ملفات Features
    features/register.cpp
    features/login.cpp
    features/booking.cpp
    features/cancel_appointment.cpp
    features/reschedule.cpp
    features/queue_managment.cpp
    features/payment.cpp
    features/doctor_schedule.cpp
    features/ViewAppointments.cpp
    features/notification.cpp

    # ملفات Database
    database/db_connection.cpp
    database/sqlite3.c 
)

# إضافة الهدف القابل للتنفيذ (Executable) وربط جميع الملفات المصدر به
add_executable(clinic ${SOURCE_FILES})