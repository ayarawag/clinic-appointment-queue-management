#include "../models/appointment.h"
#include <iostream>
#include <string>
#include <limits>     // ← هذا هو المهم
#include <vector>

void cancelAppointmentCLI() {
    int id; std::cout << "Appointment ID: "; std::cin >> id;
    Appointment a = Appointment::loadById(id);
    if (a.id == 0) { std::cout << "Appointment not found.\n"; return; }
    a.id = id;
    if (a.cancel()) std::cout << "Appointment canceled.\n";
    else std::cout << "Cancel failed.\n";
}

void rescheduleAppointmentCLI() {
    int id; std::string newTime;
    std::cout << "Appointment ID: "; std::cin >> id;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "New Time (YYYY-MM-DD HH:MM): "; std::getline(std::cin, newTime);

    Appointment a = Appointment::loadById(id);
    if (a.id == 0) { std::cout << "Appointment not found.\n"; return; }
    a.id = id;
    if (a.reschedule(newTime)) std::cout << "Rescheduled successfully.\n";
    else std::cout << "Reschedule failed (slot may be taken or DB error).\n";
}