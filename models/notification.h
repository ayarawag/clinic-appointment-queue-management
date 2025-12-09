#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

class Notification {
public:
    int appointmentId;
    std::string message;

    Notification();
    Notification(int aid, std::string msg);

    bool sendNotification(const std::string& db = "clinic.db");
    static void runReminders(const std::string& db = "clinic.db");
};

#endif