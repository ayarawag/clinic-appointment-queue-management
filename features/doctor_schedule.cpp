#include "../models/doctor.h"
#include <iostream>
#include <string>
#include <limits>

void doctorAddCLI() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string name, spec;
    std::cout << "Doctor name: ";
    std::getline(std::cin, name);

    std::cout << "Specialization: ";
    std::getline(std::cin, spec);

    // نستخدم الconstructor الجديد
    Doctor d(name, spec);

    if (d.registerDoctor("clinic.db"))
        std::cout << "Doctor added successfully.\n";
    else
        std::cout << "Failed to add doctor.\n";
}

void setDoctorScheduleCLI() {
    int id;
    std::cout << "Doctor ID: ";
    std::cin >> id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string s;
    std::cout << "Schedule (e.g. Mon-Fri 09:00-13:00): ";
    std::getline(std::cin, s);

    Doctor d = Doctor::loadById(id);

    if (d.id == 0) {
        std::cout << "Doctor not found.\n";
        return;
    }

    if (d.setSchedule(s, "clinic.db"))
        std::cout << "Schedule updated.\n";
    else
        std::cout << "Failed to update schedule.\n";
}