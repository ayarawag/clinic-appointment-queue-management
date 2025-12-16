#include <gtest/gtest.h>
#include "../models/patient.h"
#include "../database/db_connection.h"
#include "../utils/password_utils.h"
#include "../utils/time_utils.h"
#include <fstream>
#include <sstream>

// اسم قاعدة البيانات التي سيتم استخدامها في الاختبارات فقط
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات
inline void initialize_test_db(const std::string& db_name) {
    // 1. حذف الملف القديم لضمان بداية نظيفة (اختياري لكن موصى به)
    std::remove(db_name.c_str()); 
    
    // 2. قراءة محتوى سكريبت SQL
    // ملاحظة: المسار هنا هو افتراض لوجود database.sql في مجلد /database
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        // 3. فتح قاعدة البيانات وتنفيذ السكريبت
        DBConnection db(db_name);
        bool ok = db.execute(sql_script);
        
        // يمكننا التحقق من النجاح في بيئة الإنتاج، لكن هنا نعتمد على أن DBConnection تطبع الخطأ
    } else {
        // إذا لم يتم العثور على الملف، نطبع خطأ (مهم للاختبار)
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

    // يمكن إضافة دالة TearDown لحذف الملف test_clinic.db بعد كل اختبار
    void TearDown() override {
        // حذف قاعدة البيانات بعد الانتهاء
        // std::remove(TEST_DB.c_str()); 
    }
};


// --------------------------------------------------------
// الآن نستخدم TEST_F بدلاً من TEST
// --------------------------------------------------------

TEST_F(PatientTests, Registration) {
    Patient p("TestName", "0912345678", "coverage_test@example.com", "mypassword");
    
    // يجب استخدام TEST_DB بدلاً من clinic.db
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