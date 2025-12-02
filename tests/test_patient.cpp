#include <gtest/gtest.h>
#include "../models/patient.h"
#include "../database/db_connection.h"
#include <fstream>

// -------- Helper function: clean DB before each test -------
void resetPatientsTable() {
    DBConnection db("clinic.db");
    db.execute("DELETE FROM patients;");
}

// ----------- Test Suite for Patient Class ------------------
class PatientTest : public ::testing::Test {
protected:
    void SetUp() override {
        resetPatientsTable();
    }
};

// -----------------------------------------------------------
// Test 1: رفض الإيميل الغلط
TEST_F(PatientTest, InvalidEmailRejected) {
    Patient p("Sara", "0911111111", "wrong-email", "password123");
    bool result = p.registerPatient();

    EXPECT_FALSE(result);
}

// -----------------------------------------------------------
// Test 2: رفض الباسورد القصير
TEST_F(PatientTest, ShortPasswordRejected) {
    Patient p("Ali", "0922222222", "test@test.com", "123");
    bool result = p.registerPatient();

    EXPECT_FALSE(result);
}

// -----------------------------------------------------------
// Test 3: تسجيل ناجح
TEST_F(PatientTest, SuccessfulRegistration) {
    Patient p("Mira", "0933333333", "mira@test.com", "password123");

    bool result = p.registerPatient();
    EXPECT_TRUE(result);
}

// -----------------------------------------------------------
// Test 4: تسجيل دخول ناجح
TEST_F(PatientTest, SuccessfulLogin) {
    Patient p("Lama", "0944444444", "lama@test.com", "mypassword123");
    p.registerPatient();

    Patient loginTest;
    bool result = loginTest.loginPatient("lama@test.com", "mypassword123");
    EXPECT_TRUE(result);
}

// -----------------------------------------------------------
// Test 5: زيادة failedAttempts لما الباسورد غلط
TEST_F(PatientTest, WrongPasswordIncreasesFailedAttempts) {
    Patient p("Omar", "0955555555", "omar@test.com", "correctpass");
    p.registerPatient();

    Patient loginTest;

    // محاولة فاشلة 1
    loginTest.loginPatient("omar@test.com", "wrongpass");

    // نتحقق من قيمة failedAttempts
    DBConnection db("clinic.db");
    int failed = 0;
    db.query("SELECT failedAttempts FROM patients WHERE email='omar@test.com';",
        [](void* ud, int, char** vals, char**) {
            *(int*)ud = std::stoi(vals[0]);
            return 0;
        }, &failed);

    EXPECT_EQ(failed, 1);
}

// -----------------------------------------------------------
// Test 6: بعد 5 مرات غلط — ينقفل الحساب 15 دقيقة
TEST_F(PatientTest, LocksAfterFiveAttempts) {
    Patient p("Nora", "0966666666", "nora@test.com", "rightpass");
    p.registerPatient();

    Patient loginTest;

    // 5 محاولات فاشلة
    for (int i = 0; i < 5; i++)
        loginTest.loginPatient("nora@test.com", "wrongpass");

    // نقرأ lockedUntil
    DBConnection db("clinic.db");
    long long locked = 0;
    db.query("SELECT lockedUntil FROM patients WHERE email='nora@test.com';",
        [](void* ud, int, char** vals, char**) {
            *(long long*)ud = std::stoll(vals[0]);
            return 0;
        }, &locked);

    long long now = Patient::nowEpoch();
    EXPECT_GT(locked, now);  // lockedUntil > now → الحساب مقفول فعلاً
}

// -----------------------------------------------------------
// Test 7: نجاح تسجيل الدخول يعيد failedAttempts = 0
TEST_F(PatientTest, ResetFailedAfterSuccess) {
    Patient p("Rama", "0977777777", "rama@test.com", "ramapass123");
    p.registerPatient();

    Patient loginTest;

    // محاولة فاشلة
    loginTest.loginPatient("rama@test.com", "wrong");

    // محاولة ناجحة
    loginTest.loginPatient("rama@test.com", "ramapass123");

    // قراءة failedAttempts
    DBConnection db("clinic.db");
    int failed = -1;
    db.query("SELECT failedAttempts FROM patients WHERE email='rama@test.com';",
        [](void* ud, int, char** vals, char**) {
            *(int*)ud = std::stoi(vals[0]);
            return 0;
        }, &failed);

    EXPECT_EQ(failed, 0);
}