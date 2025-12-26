#include <gtest/gtest.h>
#include "../models/patient.h"
#include "../database/db_connection.h"
#include <iostream>
#include <string>
#include <vector>


using std::string;

// المسار المطلق لملف قاعدة البيانات في جهازك لضمان الوصول إليه دائماً
const std::string REAL_DB = "/mnt/c/Users/ECS/Documents/GitHub/clinic-appointment-queue-management/clinic.db";

class PatientScenariosTests : public ::testing::Test {
protected:
    void SetUp() override {
        // إنشاء نسخة الاتصال بقاعدة البيانات قبل كل اختبار
        DBConnection::getInstance(REAL_DB);
    }
    void TearDown() override {
        // تدمير نسخة الاتصال لضمان نظافة البيانات بين الاختبارات
        DBConnection::destroyInstance();
    }
};

// 1. سيناريو منع تكرار الإيميل 
// تم استخدام bb@gmail.com لأنه الإيميل الفعلي الموجود في قاعدة بياناتك
TEST_F(PatientScenariosTests, Scenario_BlockDuplicateEmail) {
    std::cout << "[SCENARIO] Attempt to register a patient with an existing email (bb@gmail.com)" << std::endl;
    
    Patient p("New User", "0910000000", "bb@gmail.com", "pass123");
    bool result = p.registerPatient(REAL_DB);
    
    // يتوقع أن يرجع الكود false لأن الإيميل موجود
    EXPECT_FALSE(result) << "خطأ: النظام سمح بتكرار إيميل موجود مسبقاً!";
}

// 2. سيناريو تحميل بيانات المريض الأول (Data Integrity)
// تم تعديل التوقعات لتطابق البيانات الفعلية (Name: jj, Email: bb@gmail.com)
TEST_F(PatientScenariosTests, Scenario_LoadPatientData) {
    std::cout << "[SCENARIO] Loading patient data number 1 and verifying its accuracy" << std::endl;
    Patient p = Patient::loadById(1, REAL_DB);
    
    ASSERT_NE(p.id, 0) << "خطأ: لم يتم العثور على المريض رقم 1 في القاعدة!";
    EXPECT_EQ(p.email, "bb@gmail.com");
    EXPECT_EQ(p.name, "jj");
}

// 3. سيناريو تحديث بيانات مريض (Update Verification)
TEST_F(PatientScenariosTests, Scenario_UpdatePatientPhone) {
    std::cout << "[SCENARIO] Testing phone number update for patient number 1" << std::endl;
    
    Patient p = Patient::loadById(1, REAL_DB);
    ASSERT_NE(p.id, 0);
    
    std::string originalPhone = p.phone; 
    p.phone = "0999999999"; 
    
    bool updateOk = p.update(REAL_DB);
    EXPECT_TRUE(updateOk);
    
    // التحقق من أن القيمة الجديدة تم حفظها فعلياً
    Patient checkP = Patient::loadById(1, REAL_DB);
    EXPECT_EQ(checkP.phone, "0999999999");
    
    // إرجاع القيمة الأصلية للحفاظ على سلامة البيانات الأساسية
    p.phone = originalPhone;
    p.update(REAL_DB);
}

// 4. سيناريو الحماية عند مسار قاعدة بيانات خاطئ (Robustness)
TEST_F(PatientScenariosTests, Scenario_DatabaseErrorHandling) {
    std::cout << "[SCENARIO] Testing handling an invalid database path" << std::endl;
    Patient p;
    // مسار وهمي لا يمكن الوصول إليه
    bool result = p.emailExists("any@test.com", "/invalid/path/db.db");
    
    // الكود يجب أن يمسك الاستثناء (Try/Catch) ويرجع false دون انهيار البرنامج
    EXPECT_FALSE(result);
}

// 5. سيناريو التحقق من تشفير كلمة المرور (Security Check)
TEST_F(PatientScenariosTests, Scenario_PasswordHashIntegrity) {
    std::cout << "[SCENARIO] Ensure the password is encrypted before storing it" << std::endl;
    string plainPass = "secret123";
    Patient p("SecurityUser", "000", "security@test.com", plainPass);
    
    // الهاش يجب ألا يساوي النص الواضح بأي حال من الأحوال
    EXPECT_NE(p.passwordHash, plainPass);
    EXPECT_FALSE(p.passwordHash.empty());
}

// 6. سيناريو منع التسجيل ببيانات فارغة (Input Validation)
TEST_F(PatientScenariosTests, Scenario_BlockEmptyData) {
    std::cout << "[SCENARIO] Attempt to register a patient with empty data" << std::endl;
    Patient emptyP("", "", "", "");
    bool result = emptyP.registerPatient(REAL_DB);
    
    EXPECT_FALSE(result) << "خطأ: النظام سمح بتسجيل مريض ببيانات ناقصة!";
}