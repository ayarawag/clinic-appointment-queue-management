#include "../models/patient.h"
#include <iostream>

bool loginUser() {
    std::string email, pw;
    std::cout << "Email/Phone: "; std::cin >> email;
    std::cout << "Password: "; std::cin >> pw;

    Patient p;
    if(p.loginPatient(email, pw)) {
        std::cout << "Login Successful!\n";
        return true;
    } else {
        std::cout << "Login Failed!\n";
        return false;
    }
}