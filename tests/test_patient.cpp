#include <iostream>
#include "../models/patient.h"
#include "../database/db_connection.h"
#include "../utils/password_utils.h"
#include "../utils/time_utils.h"

void test_patient_registration() {
    std::cout << "[TEST] Patient Registration...\n";

    // إضافة مريض جديد
    Patient p("TestName", "0912345678", "test_coverage@example.com", "mypassword");

    bool ok = p.registerPatient("clinic.db");
    if (ok)
        std::cout << "PASS: Patient registered.\n";
    else
        std::cout << "FAIL: Registration failed (email exists?)\n";
}

void test_patient_load() {
    std::cout << "[TEST] Load Patient...\n";

    Patient p = Patient::loadById(1);

    if (p.id == 1)
        std::cout << "PASS: Loaded patient.\n";
    else
        std::cout << "FAIL: Could not load patient.\n";
}

void test_patient_update() {
    std::cout << "[TEST] Update Patient...\n";

    Patient p = Patient::loadById(1);
    p.name = "NewName";

    bool ok = p.update("clinic.db");

    if (ok)
        std::cout << "PASS: Updated.\n";
    else
        std::cout << "FAIL: Update failed.\n";
}

void test_patient_delete() {
    std::cout << "[TEST] Delete Patient...\n";

    Patient p = Patient::loadById(1);

    bool ok = p.remove("clinic.db");

    if (ok)
        std::cout << "PASS: Deleted.\n";
    else
        std::cout << "FAIL: Delete failed.\n";
}

int main() {
    std::cout << "===== Running Patient Tests =====\n";

    test_patient_registration();
    test_patient_load();
    test_patient_update();
    test_patient_delete();

    std::cout << "===== Patient Tests Completed =====\n";
    return 0;
}