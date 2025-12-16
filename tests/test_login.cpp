#include <gtest/gtest.h> 
#include "../models/patient.h"
#include "../database/db_connection.h"
#include "../utils/password_utils.h" // نحتاجها للتحقق من المنطق
#include <fstream>
#include <sstream>
#include <string>

// اسم قاعدة البيانات المستخدمة في الاختبارات
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (لضمان بيئة نظيفة)
inline void initialize_test_db(const std::string& db_name) {
    std::remove(db_name.c_str()); 
    
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        DBConnection db(db_name);
        db.execute(sql_script);
    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}

// دالة محاكاة لتسجيل الدخول (تحاكي منطق features/login.cpp ولكن بدون I/O)
bool attemptLogin(const std::string& email, const std::string& password, const std::string& db) {
    DBConnection db_conn(db);
    std::string storedHash = "";

    // 1. استرجاع الهاش المخزن
    std::string q = 
        "SELECT password_hash FROM patients WHERE email='" + email + "' LIMIT 1;";

    db_conn.query(q,
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
}


// كلاس الاختبار LoginTests
class LoginTests : public ::testing::Test {
protected:
    const std::string VALID_EMAIL = "login@test.com";
    const std::string VALID_PASS  = "SecurePass123";

    void SetUp() override {
        initialize_test_db(TEST_DB); 

        // 1. تسجيل مستخدم لغرض الاختبار
        Patient p("Login User", "0910000001", VALID_EMAIL, VALID_PASS);
        p.registerPatient(TEST_DB);
        
        // يجب أن ينجح التسجيل لكي تعمل الاختبارات اللاحقة
        ASSERT_TRUE(p.id != 0); 
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