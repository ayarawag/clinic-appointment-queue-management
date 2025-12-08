#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>

class Notification {
public:
    static bool sendNotification(int patientId, const std::string& msg, const std::string& db="clinic.db");
    static void runReminders(int minutesBefore, const std::string& db="clinic.db");
};

#endif