#include "../models/notification.h"
#include <iostream>
#include <string>
#include <limits>     // ← هذا هو المهم
#include <vector>

void sendNotificationCLI() {
    int pid;
    std::cout << "Patient ID: "; std::cin >> pid;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string msg;
    std::cout << "Message: "; std::getline(std::cin, msg);
    // In our model Notification::send prints (demo) — use it
    Notification::sendNotification(pid, msg, "clinic.db");
}

void runRemindersCLI() {
    int mins; std::cout << "Minutes before appointment to notify: "; std::cin >> mins;
    Notification::runReminders(mins, "clinic.db");
}