#include "../models/patient.h"
#include <iostream>
#include <string>
#include <limits>     // ← هذا هو المهم
#include <vector>
void registerUser() {
    std::string name, phone, email, password;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Name: "; std::getline(std::cin, name);
    std::cout << "Phone: "; std::getline(std::cin, phone);
    std::cout << "Email: "; std::getline(std::cin, email);
    std::cout << "Password: "; std::getline(std::cin, password);

    if (email.empty() || password.size() < 8) {
        std::cout << "Invalid input: email required and password must be >= 8 chars\n";
        return;
    }

    Patient p(name, phone, email, password);
    if (p.registerPatient()) std::cout << "Registered successfully.\n";
    else std::cout << "Registration failed (email exists or DB error).\n";
}