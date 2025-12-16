#include <gtest/gtest.h> 
#include "../models/patient.h"
#include "../database/db_connection.h"
#include <fstream>
#include <sstream>
#include <string>

// اسم قاعدة البيانات التي سيتم استخدامها في الاختبارات فقط
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (نفس الدالة التي تستخدمها في الملفات الأخرى)
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
        DBConnection db(db_name);
        db.execute(sql_script);
    } else {
        // إذا لم يتم العثور على الملف، لا يمكننا التهيئة
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}


// كلاس الاختبار RegisterTests
class RegisterTests : public ::testing::Test {
protected:
    // يتم استدعاء SetUp قبل كل اختبار
    void SetUp() override {
        // ننظف قاعدة البيانات قبل كل اختبار
        initialize_test_db(TEST_DB); 
    }
};

// --------------------------------------------------------
// الاختبارات
// --------------------------------------------------------

// 1. اختبار التسجيل الناجح
TEST_F(RegisterTests, RegisterSuccess) {
    // نستخدم حساب وهمي
    std::string name = "TestUser";
    std::string phone = "0910000000";
    std::string email = "test_register@example.com";
    std::string pass = "mypassword123";

    Patient p(name, phone, email, pass);
    
    // نتوقع أن ينجح التسجيل
    bool ok = p.registerPatient(TEST_DB);

    // التحقق من النجاح
    EXPECT_TRUE(ok) << "Registration should succeed for a new user.";
    
    // (اختياري) يمكننا التحقق من وجود المستخدم في قاعدة البيانات هنا
}


// 2. اختبار تكرار البريد الإلكتروني (Duplicate Email)
TEST_F(RegisterTests, RegisterDuplicateEmail) {
    std::string email = "dup@example.com";

    // التسجيل الأول (نتوقع النجاح)
    Patient p1("User1", "0901111111", email, "pass1");
    bool ok1 = p1.registerPatient(TEST_DB);
    EXPECT_TRUE(ok1) << "First registration should succeed.";

    // التسجيل الثاني بنفس الإيميل (نتوقع الفشل)
    Patient p2("User2", "0902222222", email, "pass2");
    bool ok2 = p2.registerPatient(TEST_DB);

    // التحقق من الفشل (أن الدالة ترجع false)
    EXPECT_FALSE(ok2) << "Duplicate email registration should fail.";
}


// 3. اختبار التسجيل ببيانات غير مكتملة (اختياري)
TEST_F(RegisterTests, RegisterIncompleteData) {
    // محاولة التسجيل باسم فارغ (بافتراض أن الدالة تمنع ذلك)
    Patient p("BadUser", "0910000000", "", "pass1"); // إيميل فارغ
    
    // نتوقع الفشل
    bool ok = p.registerPatient(TEST_DB);

    EXPECT_FALSE(ok) << "Registration with empty email/data should fail.";
}