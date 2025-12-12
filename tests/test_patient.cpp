#include <gtest/gtest.h>
#include "../models/patient.h"
#include "../database/db_connection.h"
#include "../utils/password_utils.h"
#include "../utils/time_utils.h"

TEST(PatientTests, Registration) {
    Patient p("TestName", "0912345678", "coverage_test@example.com", "mypassword");
    bool ok = p.registerPatient("clinic.db");
    EXPECT_TRUE(ok);
}

TEST(PatientTests, Load) {
    Patient p = Patient::loadById(1, "clinic.db");
    EXPECT_EQ(p.id, 1);
}

TEST(PatientTests, Update) {
    Patient p = Patient::loadById(1, "clinic.db");
    p.name = "UpdatedName";
    bool ok = p.update("clinic.db");
    EXPECT_TRUE(ok);
}

TEST(PatientTests, Delete) {
    Patient p = Patient::loadById(1, "clinic.db");
    bool ok = p.remove("clinic.db");
    EXPECT_TRUE(ok);
}