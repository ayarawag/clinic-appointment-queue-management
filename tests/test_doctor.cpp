#include <gtest/gtest.h>
#include "../models/doctor.h"
#include "../database/db_connection.h" 
#include <fstream>
#include <sstream>
#include <iostream> 

// اسم قاعدة البيانات التي سيتم استخدامها في الاختبارات فقط
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (مع تحديث Singleton)
inline void initialize_test_db(const std::string& db_name) {
    // [الحل]: تدمير النسخة قبل محاولة حذف الملف
    DBConnection::destroyInstance(); 
    
    // 1. حذف الملف القديم لضمان بداية نظيفة
    std::remove(db_name.c_str()); 
    
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        // 2. استخدام Singleton للحصول على اتصال وتنفيذ السكريبت
        DBConnection* db = DBConnection::getInstance(db_name); 
        db->execute(sql_script);

    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}


// كلاس الاختبار DoctorTests
class DoctorTests : public ::testing::Test {
protected:
    void SetUp() override {
        // تهيئة قاعدة البيانات قبل كل اختبار
        initialize_test_db(TEST_DB); 
    }
    
    void TearDown() override {
        // [الحل]: تدمير نسخة Singleton بعد كل اختبار
        DBConnection::destroyInstance();
    }
};

// --------------------------------------------------------
// الاختبارات 
// --------------------------------------------------------

TEST_F(DoctorTests, RegistrationAndLoad) {
    Doctor d_reg("Dr. Ali", "Cardiologist");
    
    // التسجيل
    bool ok_reg = d_reg.registerDoctor(TEST_DB);
    EXPECT_TRUE(ok_reg);

    // التحميل (ID=1)
    Doctor d_load = Doctor::loadById(1, TEST_DB); 
    
    EXPECT_EQ(d_load.id, 1);
    EXPECT_EQ(d_load.name, "Dr. Ali");
    EXPECT_EQ(d_load.specialization, "Cardiologist");
}

TEST_F(DoctorTests, SetSchedule) {
    Doctor d_reg("Dr. Badr", "Surgeon");
    d_reg.registerDoctor(TEST_DB);
    
    // نحتاج لتحميله مرة أخرى للحصول على ID صحيح من قاعدة البيانات
    Doctor d_load = Doctor::loadById(1, TEST_DB); 
    std::string new_schedule = "Mon 9:00-14:00";
    
    // تعيين الجدول
    bool ok_set = d_load.setSchedule(new_schedule, TEST_DB);
    EXPECT_TRUE(ok_set);

    // التحقق من التحديث
    Doctor d_check = Doctor::loadById(1, TEST_DB);
    EXPECT_EQ(d_check.schedule, new_schedule);
}

TEST_F(DoctorTests, UpdateAndRemove) {
    Doctor d_reg("Dr. Omar", "Pediatrician");
    d_reg.registerDoctor(TEST_DB);
    
    Doctor d = Doctor::loadById(1, TEST_DB);

    // التحديث
    d.name = "Dr. Omar Updated";
    bool ok_update = d.update(TEST_DB);
    EXPECT_TRUE(ok_update);
    
    // التحقق من التحديث
    Doctor d_check = Doctor::loadById(1, TEST_DB);
    EXPECT_EQ(d_check.name, "Dr. Omar Updated");

    // الحذف
    bool ok_remove = d_check.remove(TEST_DB);
    EXPECT_TRUE(ok_remove);
    
    // التحقق من الحذف (يجب أن يعود ID=0)
    Doctor d_deleted = Doctor::loadById(1, TEST_DB);
    EXPECT_EQ(d_deleted.id, 0);
}