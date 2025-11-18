#include "../models/patient.h"
#include <iostream>

void registerUser() {
    std::string name, phone, email, password;
    std::cout << "Name: "; std::cin >> name;
    std::cout << "Phone: "; std::cin >> phone;
    std::cout << "Email: "; std::cin >> email;
    std::cout << "Password: "; std::cin >> password;

    Patient p(name, phone, email, password);
    if(p.registerPatient()) std::cout << "Registered!\n";
}