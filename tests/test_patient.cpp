#include <gtest/gtest.h>
#include "../models/patient.h"
#include "../database/db_connection.h"
#include <iostream>
#include <string>

/**
 * ملف اختبارات المريض النهائي - متوافق تماماً مع كود كلاس Patient الخاص بك
 * الهدف: اختبار الربط مع قاعدة البيانات الحقيقية clinic.db والتأكد من منع التكرار
 */

// مسار قاعدة البيانات الحقيقية التي تحتوي على بياناتك
const std::string REAL_DB = "clinic.db";

class PatientIntegrationTests : public ::testing::Test {
protected:
    void SetUp() override {
        // التأكد من فتح اتصال بقاعدة البيانات قبل كل اختبار باستخدام Singleton
        DBConnection::getInstance(REAL_DB);
    }

    void TearDown() override {
        // تدمير النسخة لضمان نظافة الاتصال بين الاختبارات
        DBConnection::destroyInstance();
    }
};

// 1. الاختبار الأهم للدكتور: منع تسجيل إيميل موجود مسبقاً (Unique Check)
TEST_F(PatientIntegrationTests, PreventRegistrationOfExistingEmail) {
    // الإيميل الموجود فعلياً في قاعدة بياناتك (miral)
    std::string existingEmail = "bb@gmail.com";
    
    std::cout << "[TEST] Attempting to register with existing email: " << existingEmail << std::endl;

    // إنشاء كائن مريض جديد بنفس الإيميل
    // نستخدم بيانات وهمية للباقي لأن الهدف هو اختبار منع تكرار الإيميل
    Patient dupPatient("Test User", "0000000000", existingEmail, "pass123");

    // استدعاء الدالة registerPatient التي تحتوي على التحقق من التكرار داخلها
    bool result = dupPatient.registerPatient(REAL_DB);

    // النتيجة المتوقعة: يجب أن تكون false لأن الإيميل موجود
    EXPECT_FALSE(result) << "فشل الاختبار: النظام سمح بتكرار إيميل موجود مسبقاً في القاعدة!";
    
    if (!result) {
        std::cout << "[SUCCESS] System correctly blocked the duplicate email registration." << std::endl;
    }
}

// 2. اختبار دالة emailExists الحقيقية
TEST_F(PatientIntegrationTests, CheckIfEmailExistsInRealDB) {
    std::string email = "bb@gmail.com";
    
    Patient p;
    // استدعاء دالة emailExists التي تستخدم Singleton و Try/Catch في كودك
    bool found = p.emailExists(email, REAL_DB);
    
    EXPECT_TRUE(found) << "فشل الاختبار: لم يتم العثور على الإيميل رغم وجوده الفعلي في clinic.db";
}

// 3. اختبار تحميل بيانات مريض موجود (LoadById)
TEST_F(PatientIntegrationTests, LoadPatientFromDatabase) {
    // حسب بياناتك، miral لديها ID = 1
    int testId = 1;
    
    // استدعاء دالة loadById من كودك
    Patient p = Patient::loadById(testId, REAL_DB);
    
    // التحقق من صحة البيانات المحملة من الملف الحقيقي
    EXPECT_EQ(p.id, testId);
    EXPECT_EQ(p.email, "bb@gmail.com");
    // التحقق من الاسم (موجود في كودك كـ name)
    EXPECT_EQ(p.name, "miral");
    
    std::cout << "[INFO] Successfully loaded: " << p.name << " with ID: " << p.id << std::endl;
}

// 4. اختبار التحقق من البيانات (Validation)
TEST_F(PatientIntegrationTests, RegistrationValidationCheck) {
    // محاولة إنشاء مريض ببيانات ناقصة (إيميل فارغ)
    Patient p("NoEmailUser", "12345", "", "password");
    
    // دالة registerPatient في كودك تبدأ بالتحقق if (email.empty())
    bool result = p.registerPatient(REAL_DB);
    
    EXPECT_FALSE(result) << "فشل الاختبار: النظام سمح بتسجيل مريض ببيانات ناقصة!";
}

// 5. اختبار معالجة استثناءات قاعدة البيانات (Exception Handling)
TEST_F(PatientIntegrationTests, HandleInvalidDatabasePath) {
    std::string wrongPath = "/non/existent/path.db";
    Patient p;
    
    // دالة emailExists في كودك مغلفة بـ try/catch وترجع false عند الخطأ
    bool result = p.emailExists("any@email.com", wrongPath);
    
    EXPECT_FALSE(result) << "فشل الاختبار: الكود لم يتعامل بشكل صحيح مع مسار قاعدة البيانات الخاطئ";
    std::cout << "[INFO] Exception handling verified (returned false instead of crashing)." << std::endl;
}