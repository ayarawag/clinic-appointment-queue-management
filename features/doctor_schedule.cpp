#include "../models/doctor.h"
#include <iostream>
#include <string>
#include <limits>     // ← هذا هو المهم
#include <vector>

void doctorAddCLI() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string name, spec;
    std::cout << "Doctor name: "; std::getline(std::cin, name);
    std::cout << "Specialization: "; std::getline(std::cin, spec);
    Doctor d(name, spec);
    if (d.create()) std::cout << "Doctor added with success.\n";
    else std::cout << "Failed to add doctor.\n";
}

void setDoctorScheduleCLI() {
    int id; std::cout << "Doctor ID: "; std::cin >> id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string s; std::cout << "Schedule (e.g. Mon-Fri 09:00-13:00): "; std::getline(std::cin, s);
    Doctor d = Doctor::loadById(id);
    if (d.id == 0) { std::cout << "Doctor not found.\n"; return; }
    d.setSchedule(s);
    std::cout << "Schedule updated.\n";
}