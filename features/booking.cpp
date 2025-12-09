#include "../models/appointment.h"
#include <iostream>
#include <string>
#include <limits>     // ← هذا هو المهم
#include <vector>

void bookAppointmentCLI() {
    Appointment a;
    std::cout << "Patient ID: "; std::cin >> a.patientId;
    std::cout << "Doctor ID: "; std::cin >> a.doctorId;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Date & Time (YYYY-MM-DD HH:MM): ";
    std::getline(std::cin, a.dateTime);

    if (a.patientId <= 0 || a.doctorId <= 0||  a.dateTime.empty()) {
        std::cout << "Invalid input.\n";
        return;
    }

    if (a.book()) std::cout << "Appointment booked successfully.\n";
    else std::cout << "Booking failed (slot may be taken or DB error).\n";
}