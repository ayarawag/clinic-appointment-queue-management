#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

class Notification {
public:
    int id;
    int appointmentId;
    std::string message;
    std::string createdAt;

    Notification();
    Notification(int appId, std::string msg);

    bool sendNotification(const std::string& db="clinic.db");
    static void runReminders(const std::string& db="clinic.db");
};

#endif