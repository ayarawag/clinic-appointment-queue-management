#include <gtest/gtest.h> 
#include "../models/patient.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include "../utils/password_utils.h" 
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept> 
#include <iostream> 

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

        // [تعديل Singleton] استخدام getInstance بدلاً من المُنشئ
        DBConnection* db = DBConnection::getInstance(db_name); 
        
        // [تعديل Singleton] استخدام المؤشر -> لتنفيذ الدالة
        db->execute(sql_script);
        
    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}

// دالة محاكاة لتسجيل الدخول (تحديث Singleton و Try/Catch)
bool attemptLogin(const std::string& email, const std::string& password, const std::string& db) {
    std::string storedHash = "";

    try {
        // [Singleton] استخدام getInstance بدلاً من المُنشئ
        DBConnection* db_conn = DBConnection::getInstance(db); 
        
        // 1. استرجاع الهاش المخزن
        std::string q = 
            "SELECT password_hash FROM patients WHERE email='" + email + "' LIMIT 1;";

        // [Singleton] استخدام المؤشر -> للاستعلام
        db_conn->query(q,
            [](void* out, int cols, char** vals, char**) -> int {
                if (vals[0])
                    *((std::string*)out) = vals[0];
                return 0;
            },
            &storedHash
        );

        // 2. التحقق من وجود الإيميل
        if (storedHash.empty()) {
            return false;
        }

        // 3. مقارنة كلمة السر
        return PasswordUtils::verifyPassword(password, storedHash);
        
    } catch (const std::exception& e) {
        // معالجة خطأ DB في سياق الاختبار
        std::cerr << "EXCEPTION CAUGHT (AttemptLogin): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}


// كلاس الاختبار LoginTests
class LoginTests : public ::testing::Test {
protected:
    const std::string VALID_EMAIL = "login@test.com";
    const std::string VALID_PASS  = "SecurePass123";

    void SetUp() override {
        initialize_test_db(TEST_DB); 

        // 1. تسجيل مستخدم لغرض الاختبار (يجب أن تستخدم registerPatient Singleton الآن)
        Patient p("Login User", "0910000001", VALID_EMAIL, VALID_PASS);
        p.registerPatient(TEST_DB);
        
        ASSERT_TRUE(p.id != 0); 
    }
    
    void TearDown() override {
        // [ملاحظة Singleton] يجب استدعاء دالة التدمير هنا إذا كانت موجودة لضمان عزل الاختبارات
        // DBConnection::destroyInstance();
    }
};

// --------------------------------------------------------
// الاختبارات
// --------------------------------------------------------

// 1. اختبار تسجيل الدخول الناجح
TEST_F(LoginTests, SuccessfulLogin) {
    bool login_ok = attemptLogin(VALID_EMAIL, VALID_PASS, TEST_DB);
    
    EXPECT_TRUE(login_ok) << "Login should succeed with correct credentials.";
}

// 2. اختبار كلمة مرور خاطئة
TEST_F(LoginTests, IncorrectPassword) {
    bool login_ok = attemptLogin(VALID_EMAIL, "WrongPassword", TEST_DB);
    
    EXPECT_FALSE(login_ok) << "Login should fail with an incorrect password.";
}

// 3. اختبار بريد إلكتروني غير موجود
TEST_F(LoginTests, NonExistentEmail) {
    bool login_ok = attemptLogin("nonexistent@test.com", VALID_PASS, TEST_DB);
    
    EXPECT_FALSE(login_ok) << "Login should fail for a non-existent email.";
}