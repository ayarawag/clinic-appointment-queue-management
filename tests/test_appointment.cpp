#include <gtest/gtest.h>
#include "../models/appointment.h"
#include "../models/patient.h"
#include "../models/doctor.h"
#include "../database/db_connection.h" 
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

// اسم قاعدة البيانات التي سيتم استخدامها في الاختبارات فقط
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (مع استخدام inline لمنع خطأ التعريفات المتعددة)
inline void initialize_test_db(const std::string& db_name) {
    DBConnection::destroyInstance(); 
    std::remove(db_name.c_str()); 
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();
        DBConnection* db = DBConnection::getInstance(db_name); 
        db->execute(sql_script); 
    } else {
        std::cerr<<"failed"<< std::endl;
    }
}
// دالة مساعدة لحساب عدد المواعيد 
inline int getAppointmentCount(const std::string& db_name) {
    int count = 0;
    DBConnection* conn = DBConnection::getInstance(db_name);
    if (!conn) return -1; 
    
    conn->query(
        "SELECT COUNT(*) FROM appointments;",
        [](void* data, int argc, char** argv, char** col_names) -> int {
            if (argv[0]) {
                try {
                    *((int*)data) = std::stoi(argv[0]);
                } catch (...) {}
            }
            return 0;
        },
        &count
    );
    return count;
}


// كلاس الاختبار AppointmentTests
class AppointmentTests : public ::testing::Test {
protected:
    void SetUp() override {
        // [الحل]: يتم استدعاء initialize_test_db التي تقوم بتدمير النسخة القديمة
        initialize_test_db(TEST_DB); 

        // تسجيل المستخدمين (يجب أن يتم بنجاح الآن)
        Patient p("ApptPatient", "100", "p@a.com", "pass");
        ASSERT_TRUE(p.registerPatient(TEST_DB)); 
        
        Doctor d("Dr. Appt", "Generalist");
        ASSERT_TRUE(d.registerDoctor(TEST_DB)); 
    }
    
    // [الحل]: يتم استدعاء TearDown بعد كل اختبار
    void TearDown() override {
        // [Singleton]: تدمير نسخة Singleton بعد كل اختبار لمنع الحظر (Locking)
        DBConnection::destroyInstance(); 
    }
};

// --------------------------------------------------------
// الاختبارات
// --------------------------------------------------------

TEST_F(AppointmentTests, BookingAndLoad) {
    int initial_count = getAppointmentCount(TEST_DB);

    // 1. الحجز
    Appointment a;
    a.patientId = 1; 
    a.doctorId = 1;  
    a.dateTime = "2025-01-01 10:00:00";

    bool ok_book = a.book(TEST_DB);
    EXPECT_TRUE(ok_book);
    EXPECT_EQ(getAppointmentCount(TEST_DB), initial_count + 1);
    
    // 2. التحميل
    Appointment a_loaded = Appointment::loadById(1, TEST_DB);
    
    EXPECT_EQ(a_loaded.id, 1);
    EXPECT_EQ(a_loaded.patientId, 1);
    EXPECT_EQ(a_loaded.dateTime, "2025-01-01 10:00:00");
}

TEST_F(AppointmentTests, Reschedule) {
    // 1. الحجز أولاً
    Appointment a_book(1, 1, "2025-01-02 09:00:00");
    ASSERT_TRUE(a_book.book(TEST_DB)); 

    Appointment a = Appointment::loadById(1, TEST_DB);
    std::string new_time = "2025-01-02 14:00:00";
    
    // 2. إعادة الجدولة
    bool ok_reschedule = a.reschedule(new_time, TEST_DB);
    EXPECT_TRUE(ok_reschedule);

    // 3. التحقق من التحديث
    Appointment a_checked = Appointment::loadById(1, TEST_DB);
    EXPECT_EQ(a_checked.dateTime, new_time);
}

TEST_F(AppointmentTests, Cancel) {
    // 1. الحجز أولاً
    int initial_count = getAppointmentCount(TEST_DB);
    Appointment a_book(1, 1, "2025-01-03 11:00:00");
    ASSERT_TRUE(a_book.book(TEST_DB));
    
    // [الحل] تحميل الكائن للحصول على ID صحيح
    Appointment a = Appointment::loadById(a_book.id, TEST_DB); 
    
    // 2. الإلغاء
    bool ok_cancel = a.
    cancel(TEST_DB);
    EXPECT_TRUE(ok_cancel);

    // 3. التحقق من التغيير (التحقق من أن الحالة أصبحت "Cancelled")
    Appointment a_cancelled = Appointment::loadById(a_book.id, TEST_DB);
    EXPECT_EQ(a_cancelled.status, "Cancelled"); 
    
    // تأكد من أن عدد المواعيد لم يتغير (لأن الإلغاء تحديث حالة)
    EXPECT_EQ(getAppointmentCount(TEST_DB), initial_count + 1);
}