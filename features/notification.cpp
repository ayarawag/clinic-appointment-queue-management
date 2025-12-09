#include "../models/notification.h"
#include <iostream>
#include <string>
#include <limits>

void sendNotificationCLI() {
    int pid;
    std::cout << "Patient ID: ";
    std::cin >> pid;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string msg;
    std::cout << "Message: ";
    std::getline(std::cin, msg);

    Notification n(pid, msg);

    if (n.sendNotification("clinic.db"))
        std::cout << "Notification sent.\n";
    else
        std::cout << "Failed to send notification.\n";
}

void runRemindersCLI() {
    std::cout << "Running reminders...\n";

    Notification::runReminders("clinic.db");
}