#include <iostream>
#include <limits>

// FEATURES
void registerUser();
bool loginUser();
void bookAppointmentCLI();
void cancelAppointmentCLI();
void rescheduleAppointmentCLI();
void setDoctorScheduleCLI();
void viewAppointmentsCLI();
void payAppointmentCLI();
void showQueueCLI();
void reorderQueueCLI();
void addToQueueCLI();
void doctorAddCLI();

int main() {
    int choice = -1;
    bool loggedIn = false;

    while (true) {
        std::cout << "\n==============================\n";
        std::cout << "   Clinic Appointment System\n";
        std::cout << "==============================\n";
        std::cout << "1. Register\n";
        std::cout << "2. Login\n";
        std::cout << "3. Book Appointment\n";
        std::cout << "4. Cancel Appointment\n";
        std::cout << "5. Reschedule Appointment\n";
        std::cout << "6. Pay for Appointment\n";
        std::cout << "7. View All Appointments\n";
        std::cout << "8. Doctor: Add New Doctor\n";
        std::cout << "9. Doctor: Set Schedule\n";
        std::cout << "10. Queue: Add to Queue\n";
        std::cout << "11. Queue: Show Queue\n";
        std::cout << "12. Queue: Reorder Queue\n";
        std::cout << "0. Exit\n";
        std::cout << "==============================\n";
        std::cout << "Enter choice: ";

        std::cin >> choice;

        if (!std::cin) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "\nInvalid input.\n";
            continue;
        }

        switch (choice) {
            case 1:
                registerUser();
                break;

            case 2:
                loggedIn = loginUser();
                break;

            case 3:
                if (!loggedIn) { std::cout << "Login required.\n"; break; }
                bookAppointmentCLI();
                break;

            case 4:
                if (!loggedIn) { std::cout << "Login required.\n"; break; }
                cancelAppointmentCLI();
                break;

            case 5:
                if (!loggedIn) { std::cout << "Login required.\n"; break; }
                rescheduleAppointmentCLI();
                break;

            case 6:
                if (!loggedIn) { std::cout << "Login required.\n"; break; }
                payAppointmentCLI();
                break;

            case 7:
                if (!loggedIn) { std::cout << "Login required.\n"; break; }
                viewAppointmentsCLI();
                break;

            case 8:
                doctorAddCLI();
                break;

            case 9:
                setDoctorScheduleCLI();
                break;

            case 10:
                addToQueueCLI();
                break;

            case 11:
                showQueueCLI();
                break;

            case 12:
                reorderQueueCLI();
                break;

            case 0:
                std::cout << "Goodbye.\n";
                return 0;

            default:
                std::cout << "Invalid choice.\n";
        }
    }

    return 0;
}