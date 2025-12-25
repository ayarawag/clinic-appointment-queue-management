#include "../models/doctor.h"
#include <iostream>
#include <string>
#include <limits>
#include <vector>


void viewAvailableSlotsCLI() {

    int doctorId;
    std::string date;

    std::cout << "\n--- View Available Appointment Slots ---\n";
    std::cout << "Enter Doctor ID: ";
    std::cin >> doctorId;

    if (!std::cin || doctorId <= 0) {
        std::cin.clear();
        std::cin.ignore(5000, '\n');
        std::cout << "Invalid doctor ID.\n";
        return;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::cout << "Enter Date (YYYY-MM-DD): ";
    std::getline(std::cin, date);

    if (date.empty()) {
        std::cout << "Date cannot be empty.\n";
        return;
    }

    // استدعاء الدالة الجديدة من Doctor
    std::vector<std::string> slots =
        Doctor::getAvailableSlots(doctorId, date);

    if (slots.empty()) {
        std::cout << "\nNo available slots for this doctor on this date.\n";
        return;
    }

    std::cout << "\nAvailable Slots:\n";
    for (const auto& s : slots) {
        std::cout << "- " << s << "\n";
    }

    std::cout << "----------------------------------------\n";
}