#include <gtest/gtest.h>
#include "../models/appointment.h"
#include "../models/patient.h"
#include "../models/doctor.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <fstream>
#include <sstream>

// اسم قاعدة البيانات التي سيتم استخدامها في الاختبارات فقط
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (مع استخدام inline لمنع خطأ التعريفات المتعددة)
inline void initialize_test_db(const std::string& db_name) {
    // 1. حذف الملف القديم لضمان بداية نظيفة
    std::remove(db_name.c_str()); 
    
    // 2. قراءة محتوى سكريبت SQL
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        // 3. فتح قاعدة البيانات وتنفيذ السكريبت
        // [تعديل Singleton] استخدام getInstance بدلاً من المُنشئ
        DBConnection* db = DBConnection::getInstance(db_name); 
        
        // [تعديل Singleton] استخدام المؤشر -> لتنفيذ الدالة
        db->execute(sql_script); 

        // [إضافة اختيارية] تدمير النسخة بعد التهيئة إذا لم تكن مطلوبة لاحقاً
        // DBConnection::destroyInstance(); 
    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}


// كلاس الاختبار AppointmentTests
class AppointmentTests : public ::testing::Test {
protected:
    // يتم استدعاء SetUp قبل كل اختبار
    void SetUp() override {
        initialize_test_db(TEST_DB); 

        // يجب تسجيل مريض وطبيب أولاً لكي تتم عملية الحجز (Foreign Key Constraints)
        // ملاحظة: إذا كانت registerPatient و registerDoctor تستخدمان Singleton
        // فيجب التأكد من أنهما تستدعيان DBConnection::getInstance(TEST_DB)
        
        Patient p("ApptPatient", "100", "p@a.com", "pass");
        p.registerPatient(TEST_DB); // هذا سيكون Patient ID 1
        
        Doctor d("Dr. Appt", "Generalist");
        d.registerDoctor(TEST_DB); // هذا سيكون Doctor ID 1
    }
    
    // [إضافة] يتم استدعاء TearDown بعد كل اختبار
    // يمكن هنا إضافة منطق لتنظيف Singleton إذا لزم الأمر
    void TearDown() override {
        // [Singleton] إذا كان لديك دالة لتدمير النسخة
        // DBConnection::destroyInstance(); 
    }
};

// --------------------------------------------------------
// الاختبارات
// --------------------------------------------------------

TEST_F(AppointmentTests, BookingAndLoad) {
    // 1. الحجز
    Appointment a;
    a.patientId = 1; // من SetUp
    a.doctorId = 1;  // من SetUp
    a.dateTime = "2025-01-01 10:00:00";

    bool ok_book = a.book(TEST_DB);
    EXPECT_TRUE(ok_book);
    
    // 2. التحميل
    Appointment a_loaded = Appointment::loadById(1, TEST_DB);
    
    EXPECT_EQ(a_loaded.id, 1);
    EXPECT_EQ(a_loaded.patientId, 1);
    EXPECT_EQ(a_loaded.dateTime, "2025-01-01 10:00:00");
    // يجب أن تكون الحالة (status) "Booked" إذا كان هذا هو الافتراضي
    // EXPECT_EQ(a_loaded.status, "Booked"); 
}

TEST_F(AppointmentTests, Reschedule) {
    // 1. الحجز أولاً
    Appointment a_book;
    a_book.patientId = 1;
    a_book.doctorId = 1;
    a_book.dateTime = "2025-01-02 09:00:00";
    a_book.book(TEST_DB);

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
    Appointment a_book;
    a_book.patientId = 1;
    a_book.doctorId = 1;
    a_book.dateTime = "2025-01-03 11:00:00";
    a_book.book(TEST_DB);

    Appointment a = Appointment::loadById(1, TEST_DB);
    
    // 2. الإلغاء
    bool ok_cancel = a.cancel(TEST_DB);
    EXPECT_TRUE(ok_cancel);

    // 3. التحقق من التغيير (إذا كانت دالة cancel تحذف الصف، فنتوقع أن يكون a_cancelled.id = 0)
    // Appointment a_cancelled = Appointment::loadById(1, TEST_DB);
    // EXPECT_EQ(a_cancelled.id, 0); 
}