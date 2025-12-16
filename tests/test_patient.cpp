#include <gtest/gtest.h>
#include "../models/patient.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include "../utils/password_utils.h"
#include "../utils/time_utils.h"
#include <fstream>
#include <sstream>
#include <iostream> 

// اسم قاعدة البيانات التي سيتم استخدامها في الاختبارات فقط
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (مع تحديث Singleton)
inline void initialize_test_db(const std::string& db_name) {
    // [الحل]: تدمير النسخة القديمة قبل محاولة حذف الملف
    DBConnection::destroyInstance(); 
    
    // 1. حذف الملف القديم لضمان بداية نظيفة
    std::remove(db_name.c_str()); 
    
    // 2. قراءة محتوى سكريبت SQL
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        // 3. فتح قاعدة البيانات وتنفيذ السكريبت
        DBConnection* db = DBConnection::getInstance(db_name);
        
        bool ok = db->execute(sql_script);
        
        if (!ok) {
            std::cerr << "WARNING: SQL script execution failed during DB initialization." << std::endl;
        }
        
        // [الحل]: تدمير النسخة بعد التهيئة
        DBConnection::destroyInstance(); 
    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}


// كلاس الاختبار الذي سيتضمن دوال التهيئة والتنظيف
class PatientTests : public ::testing::Test {
protected:
    // يتم استدعاء SetUp قبل كل اختبار
    void SetUp() override {
        // تهيئة قاعدة البيانات قبل كل اختبار
        initialize_test_db(TEST_DB); 
    }

    // يتم استدعاء TearDown بعد كل اختبار
    void TearDown() override {
        // [الحل]: استدعاء دالة التدمير هنا لضمان عزل الاختبارات
        DBConnection::destroyInstance(); 
    }
};


// --------------------------------------------------------
// الاختبارات 
// --------------------------------------------------------

TEST_F(PatientTests, Registration) {
    Patient p("TestName", "0912345678", "coverage_test@example.com", "mypassword");
    
    bool ok = p.registerPatient(TEST_DB); 
    EXPECT_TRUE(ok);

    // اختبار عدم السماح بالتسجيل مرة أخرى بنفس البريد الإلكتروني
    Patient p_dup("TestName2", "0912345679", "coverage_test@example.com", "mypassword");
    bool ok_dup = p_dup.registerPatient(TEST_DB);
    EXPECT_FALSE(ok_dup);
}

TEST_F(PatientTests, Load) {
    // يجب أولاً تسجيل مريض لنتمكن من تحميله
    Patient p_reg("LoadTest", "123", "load_test@example.com", "password");
    p_reg.registerPatient(TEST_DB);
    
    // بما أن التسجيل يتم بأرقام تصاعدية، ID=1 هو أول مريض نسجله في قاعدة البيانات هذه
    Patient p = Patient::loadById(1, TEST_DB); 
    
    EXPECT_EQ(p.id, 1);
    EXPECT_EQ(p.email, "load_test@example.com");
}

TEST_F(PatientTests, Update) {
    // التسجيل أولاً (هذا سيكون ID=1 في قاعدة بيانات الاختبار)
    Patient p_reg("UpdateTest", "456", "update_test@example.com", "password");
    p_reg.registerPatient(TEST_DB);
    
    // التحميل
    Patient p = Patient::loadById(1, TEST_DB);
    p.name = "UpdatedName";
    
    // التحديث
    bool ok = p.update(TEST_DB); 
    EXPECT_TRUE(ok);
    
    // التحقق من التحديث
    Patient p_check = Patient::loadById(1, TEST_DB);
    EXPECT_EQ(p_check.name, "UpdatedName");
}

TEST_F(PatientTests, Delete) {
    // التسجيل أولاً (هذا سيكون ID=1 في قاعدة بيانات الاختبار)
    Patient p_reg("DeleteTest", "789", "delete_test@example.com", "password");
    p_reg.registerPatient(TEST_DB);

    // التحميل
    Patient p = Patient::loadById(1, TEST_DB);
    
    // الحذف
    bool ok = p.remove(TEST_DB); 
    EXPECT_TRUE(ok);
    
    // التحقق من الحذف (بتحميله مرة أخرى، يجب أن يكون ID=0)
    Patient p_deleted = Patient::loadById(1, TEST_DB);
    EXPECT_EQ(p_deleted.id, 0);
}