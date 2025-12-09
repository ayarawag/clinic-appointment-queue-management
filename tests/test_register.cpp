#include <iostream>
using namespace std;

#include "../features/register.cpp"
#include "../models/patient.h"

void test_register_success() {
    cout << "[TEST] Register Success...\n";

    // نستخدم حساب وهمي
    string name = "TestUser";
    string phone = "0910000000";
    string email = "test_register@example.com";
    string pass = "mypassword123";

    // نتأكد ما فيش حساب بنفس الإيميل
    Patient p(name, phone, email, pass);
    bool ok = p.registerPatient("clinic.db");

    if (ok) cout << "PASS: Registration OK.\n";
    else    cout << "FAIL: Registration FAILED.\n";
}

void test_register_duplicate_email() {
    cout << "[TEST] Register Duplicate Email...\n";

    string email = "dup@example.com";

    Patient p1("User1", "0901111111", email, "pass1");
    p1.registerPatient("clinic.db");

    Patient p2("User2", "0902222222", email, "pass2");
    bool ok = p2.registerPatient("clinic.db");

    if (!ok) cout << "PASS: Duplicate email blocked.\n";
    else     cout << "FAIL: Duplicate email ALLOWED.\n";
}