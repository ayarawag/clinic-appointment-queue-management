#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

class Notification {
public:
    int patientId;
    std::string message;

    Notification();
    Notification(int pid, std::string msg);

    bool sendNotification(const std::string& db = "clinic.db");
    static void runReminders(const std::string& db = "clinic.db");
};

#endif