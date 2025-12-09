#include "../models/appointment.h"
#include <iostream>
#include <string>
#include <limits>

void cancelAppointmentCLI() {
    int id;
    std::cout << "Appointment ID: ";
    std::cin >> id;

    Appointment a = Appointment::loadById(id);

    if (a.id == 0) {
        std::cout << "Appointment not found.\n";
        return;
    }

    if (a.cancel()) {
        std::cout << "Appointment cancelled.\n";
    } else {
        std::cout << "Cancellation failed.\n";
    }
}

void rescheduleAppointmentCLI() {
    int id;
    std::cout << "Appointment ID: ";
    std::cin >> id;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string newDT;
    std::cout << "New date/time (YYYY-MM-DD HH:MM): ";
    std::getline(std::cin, newDT);

    Appointment a = Appointment::loadById(id);

    if (a.id == 0) {
        std::cout << "Appointment not found.\n";
        return;
    }

    if (a.reschedule(newDT)) {
        std::cout << "Appointment rescheduled.\n";
    } else {
        std::cout << "Reschedule failed.\n";
    }
}