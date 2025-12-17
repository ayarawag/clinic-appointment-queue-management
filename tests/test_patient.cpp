#include <gtest/gtest.h>
#include "../models/patient.h"
#include "../database/db_connection.h" // Singleton
#include "../utils/password_utils.h"
#include "../utils/time_utils.h"
#include <fstream>
#include <sstream>
#include <iostream>

// اسم قاعدة البيانات للاختبارات فقط
const std::string TEST_DB = "test_clinic.db";
// دالة تهيئة قاعدة البيانات
inline void initialize_test_db(const std::string& db_name) {
    DBConnection::destroyInstance();//singleton
    // حذف قاعدة البيانات القديمة
    std::remove(db_name.c_str());
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();
        DBConnection* db = DBConnection::getInstance(db_name);
        bool ok = db->execute(sql_script);
        if (!ok) {
            std::cerr<<"failed"<<std::endl;
        }
        DBConnection::destroyInstance();
    } else {
        std::cerr<<"database.sql not found"<<std::endl;
    }
}

// كلاس الاختبارات
class PatientTests:
public::testing::Test {
protected:
    void SetUp() override {
    initialize_test_db(TEST_DB);
    }
    void TearDown() override {
    DBConnection::destroyInstance();
    }
}; 
//tests
TEST_F(PatientTests, Registration) {
    Patient p("TestName", "0912345678", "miral@gmail.com", "mypassword");
    bool ok = p.registerPatient(TEST_DB);
    EXPECT_TRUE(ok);
    // محاولة تسجيل بنفس الإيميل
    Patient p_dup("TestName2", "0912345679", "miral@gmail.com", "mypassword");
    bool ok_dup = p_dup.registerPatient(TEST_DB);
    EXPECT_FALSE(ok_dup);
}
TEST_F(PatientTests, Load) {
    Patient p_reg("LoadTest", "123", "load_test@example.com", "password");
    p_reg.registerPatient(TEST_DB);

    Patient p = Patient::loadById(1, TEST_DB);
    EXPECT_EQ(p.id, 1);
    EXPECT_EQ(p.email, "load_test@example.com");
}
TEST_F(PatientTests, Update) {
    Patient p_reg("UpdateTest", "456", "update_test@example.com", "password");
    p_reg.registerPatient(TEST_DB);

    Patient p = Patient::loadById(1, TEST_DB);
    p.name = "UpdatedName";

    bool ok = p.update(TEST_DB);
    EXPECT_TRUE(ok);

    Patient p_check = Patient::loadById(1, TEST_DB);
    EXPECT_EQ(p_check.name, "UpdatedName");
}

TEST_F(PatientTests, Delete) {
    Patient p_reg("DeleteTest", "789", "delete_test@example.com", "password");
    p_reg.registerPatient(TEST_DB);

    Patient p = Patient::loadById(1, TEST_DB);
    bool ok = p.remove(TEST_DB);
    EXPECT_TRUE(ok);

    Patient p_deleted = Patient::loadById(1, TEST_DB);
    EXPECT_EQ(p_deleted.id, 0);
}


TEST_F(PatientTests, EmailExists_DBFailure_CatchCoverage) {
    // مسار DB غلط عمداً
    std::string invalid_db = "/invalid_path/db_does_not_exist.db";

    Patient p("FailTest", "000", "fail_test@example.com", "password");

    // لازم يدخل catch ويرجع false بدون crash
    bool exists = p.emailExists("fail_test@example.com", invalid_db);

    EXPECT_FALSE(exists);
}