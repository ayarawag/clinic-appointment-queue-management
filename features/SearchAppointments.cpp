#include "../database/db_connection.h" // Singleton DB
#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>

// ==========================================================
// Feature 11: Search Appointments
// User can search by:
// 1) Doctor ID
// 2) Date (YYYY-MM-DD)
// ==========================================================
void searchAppointmentsCLI() {

    const std::string DB_FILE = "clinic.db";
    DBConnection* db = DBConnection::getInstance(DB_FILE);

    int choice;
    std::cout << "\n--- Search Appointments ---\n";
    std::cout << "1. Search by Doctor ID\n";
    std::cout << "2. Search by Date (YYYY-MM-DD)\n";
    std::cout << "Enter choice: ";
    std::cin >> choice;

    if (!std::cin) {
        std::cin.clear();
        std::cin.ignore(5000, '\n');
        std::cout << "Invalid input.\n";
        return;
    }

    try {

        if (choice == 1) {
            int doctorId;
            std::cout << "Enter Doctor ID: ";
            std::cin >> doctorId;

            if (doctorId <= 0) {
                std::cout << "Invalid doctor ID.\n";
                return;
            }

            std::string q =
                "SELECT id, patientId, doctorId, dateTime, status, paid "
                "FROM appointments WHERE doctorId=" + std::to_string(doctorId) +
                " ORDER BY dateTime ASC;";

            std::cout << "\nResults:\n";

            db->query(
                q,
                [](void*, int, char** vals, char**) -> int {
                    std::cout
                        << "ID: "        << (vals[0] ? vals[0] : "N/A")
                        << " | Patient: " << (vals[1] ? vals[1] : "N/A")
                        << " | Doctor: "  << (vals[2] ? vals[2] : "N/A")
                        << " | Time: "    << (vals[3] ? vals[3] : "N/A")
                        << " | Status: "  << (vals[4] ? vals[4] : "N/A")
                        << " | Paid: "    << (vals[5] ? vals[5] : "0")
                        << "\n";
                    return 0;
                },
                nullptr
            );

        } 
        else if (choice == 2) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            std::string date;
            std::cout << "Enter Date (YYYY-MM-DD): ";
            std::getline(std::cin, date);

            if (date.empty()) {
                std::cout << "Date cannot be empty.\n";
                return;
            }

            std::string q =
                "SELECT id, patientId, doctorId, dateTime, status, paid "
                "FROM appointments WHERE dateTime LIKE '" + date + "%' "
                "ORDER BY dateTime ASC;";

            std::cout << "\nResults:\n";

            db->query(
                q,
                [](void*, int, char** vals, char**) -> int {
                    std::cout
                        << "ID: "        << (vals[0] ? vals[0] : "N/A")
                        << " | Patient: " << (vals[1] ? vals[1] : "N/A")
                        << " | Doctor: "  << (vals[2] ? vals[2] : "N/A")
                        << " | Time: "    << (vals[3] ? vals[3] : "N/A")
                        << " | Status: "  << (vals[4] ? vals[4] : "N/A")
                        << " | Paid: "    << (vals[5] ? vals[5] : "0")
                        << "\n";
                    return 0;
                },
                nullptr
            );
        }
        else {
            std::cout << "Invalid choice.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "CRITICAL ERROR (SearchAppointments): "
                  << e.what() << std::endl;
    }
}