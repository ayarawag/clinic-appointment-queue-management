#include <gtest/gtest.h> 
#include "../models/appointment.h"
#include "../models/patient.h"
#include "../models/doctor.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <stdexcept>

// اسم قاعدة البيانات المستخدمة في الاختبارات
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (تحديث Singleton)
inline void initialize_test_db(const std::string& db_name) {
    std::remove(db_name.c_str()); 
    
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        // [تعديل Singleton]
        DBConnection* db = DBConnection::getInstance(db_name);
        db->execute(sql_script);
    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}

// دالة مساعدة لحساب عدد المواعيد في قاعدة البيانات (تحديث Singleton و Try/Catch)
inline int getAppointmentCount(const std::string& db_name) {
    int count = 0;
    
    try {
        // [تعديل Singleton]
        DBConnection* conn = DBConnection::getInstance(db_name);
        
        // [تعديل Singleton]
        conn->query(
            "SELECT COUNT(*) FROM appointments;",
            [](void* data, int argc, char** argv, char** col_names) -> int {
                int* count_ptr = static_cast<int*>(data);
                if (argv[0]) {
                    // يجب استخدام try-catch هنا أيضاً لتحويل stoi بأمان
                    try {
                        *count_ptr = std::stoi(argv[0]);
                    } catch (const std::exception& e) {
                         std::cerr << "Error in callback stoi: " << e.what() << std::endl;
                    }
                }
                return 0;
            },
            &count
        );
        return count;
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (getAppointmentCount): " << e.what() << std::endl;
        return -1; // إرجاع قيمة خطأ
    }
}


// كلاس الاختبار BookingTests
class BookingTests : public ::testing::Test {
protected:
    int patientId;
    int doctorId;
    const std::string FUTURE_TIME = "2025-12-30 11:00:00";
    
    void SetUp() override {
        initialize_test_db(TEST_DB); 

        // 1. تسجيل مريض (يستخدم Singleton الآن)
        Patient p("BookPat", "100", "p@book.com", "pass");
        p.registerPatient(TEST_DB);
        patientId = p.id;
        
        // 2. تسجيل طبيب (يستخدم Singleton الآن)
        Doctor d("Dr. Book", "Pediatrician");
        d.registerDoctor(TEST_DB);
        doctorId = d.id;
        
        // التأكد من أن المستخدمين موجودون
        ASSERT_TRUE(patientId != 0); 
        ASSERT_TRUE(doctorId != 0); 
    }
    
    void TearDown() override {
        // [ملاحظة Singleton]
        // DBConnection::destroyInstance();
    }
};

// --------------------------------------------------------
// الاختبارات
// --------------------------------------------------------

// 1. اختبار الحجز الناجح (Successful Booking)
TEST_F(BookingTests, SuccessfulBooking) {
    int initial_count = getAppointmentCount(TEST_DB);

    Appointment a(patientId, doctorId, FUTURE_TIME);
    
    // محاولة الحجز ببيانات صالحة (تستخدم Singleton الآن)
    bool ok = a.book(TEST_DB);

    EXPECT_TRUE(ok) << "Booking should succeed with valid data.";
    
    // التحقق من أن عدد المواعيد زاد
    EXPECT_EQ(getAppointmentCount(TEST_DB), initial_count + 1);
    
    // التحقق من أن الكائن حصل على ID
    EXPECT_TRUE(a.id != 0);
}


// 2. اختبار فشل الحجز بسبب ID غير صالح أو تاريخ فارغ (Validation Check)
TEST_F(BookingTests, BookingFailsWithInvalidData) {
    int initial_count = getAppointmentCount(TEST_DB);

    // الحالة أ: PatientId غير صالح (القيمة 0 تفشل في التحقق)
    Appointment a_invalid_pat(0, doctorId, FUTURE_TIME); 
    bool ok1 = a_invalid_pat.book(TEST_DB);

    // الحالة ب: DoctorId غير صالح (القيمة 0 تفشل في التحقق)
    Appointment a_invalid_doc(patientId, 0, FUTURE_TIME); 
    bool ok2 = a_invalid_doc.book(TEST_DB);
    
    // الحالة ج: DateTime فارغ (يفشل في التحقق)
    Appointment a_empty_time(patientId, doctorId, ""); 
    bool ok3 = a_empty_time.book(TEST_DB);
    
    // نتوقع أن تفشل جميع المحاولات بسبب التحقق (Validation)
    EXPECT_FALSE(ok1) << "Booking should fail for invalid Patient ID (Validation).";
    EXPECT_FALSE(ok2) << "Booking should fail for invalid Doctor ID (Validation).";
    EXPECT_FALSE(ok3) << "Booking should fail with empty date/time (Validation).";

    // التحقق من أن عدد المواعيد لم يزد بعد المحاولات الفاشلة
    EXPECT_EQ(getAppointmentCount(TEST_DB), initial_count);
}