#include "../models/doctor.h"
#include <iostream>

void test_doctor_creation() {
    std::cout << "[TEST] Doctor Creation...\n";

    Doctor d(0, "Dr.Test", "Cardiology");
    if (d.name == "Dr.Test" && d.specialty == "Cardiology")
        std::cout << "PASS: Doctor created.\n";
    else
        std::cout << "FAIL: Doctor creation failed.\n";
}

void test_doctor_register() {
    std::cout << "[TEST] Doctor Register...\n";

    Doctor d(0, "Dr.Coverage", "Dermatology");
    bool ok = d.registerDoctor("clinic.db");

    if (ok)
        std::cout << "PASS: Doctor registered.\n";
    else
        std::cout << "FAIL: Doctor registration failed.\n";
}

void test_doctor_load() {
    std::cout << "[TEST] Load Doctor...\n";

    Doctor d = Doctor::loadById(1);

    if (d.id == 1)
        std::cout << "PASS: Loaded doctor.\n";
    else
        std::cout << "FAIL: Doctor loading failed.\n";
}

void test_doctor_update() {
    std::cout << "[TEST] Update Doctor...\n";

    Doctor d = Doctor::loadById(1);
    d.name = "Dr.NewName";

    bool ok = d.update("clinic.db");

    if (ok)
        std::cout << "PASS: Doctor updated.\n";
    else
        std::cout << "FAIL: Doctor update failed.\n";
}

void test_doctor_delete() {
    std::cout << "[TEST] Delete Doctor...\n";

    Doctor d = Doctor::loadById(1);

    bool ok = d.remove("clinic.db");

    if (ok)
        std::cout << "PASS: Doctor deleted.\n";
    else
        std::cout << "FAIL: Doctor delete failed.\n";
}