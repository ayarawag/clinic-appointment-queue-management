#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

// ================= Notification =================
class Notification {
public:
    int patientId;
    std::string message;

    Notification();
    Notification(int pid, std::string msg);

    bool sendNotification(const std::string& db = "clinic.db");
    static void runReminders(const std::string& db = "clinic.db");
};

// ================= Factory Pattern =================
class NotificationFactory {
public:
    static Notification createNotification(int pid, const std::string& msg) {
        return Notification(pid, msg);
    }
};

#endif