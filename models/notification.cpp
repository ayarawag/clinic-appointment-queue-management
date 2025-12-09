#include "notification.h"
#include "../database/db_connection.h"
#include <sstream>
#include <iostream>

Notification::Notification() : id(0), appointmentId(0) {}

Notification::Notification(int appId, std::string msg)
    : id(0), appointmentId(appId), message(msg) {}

bool Notification::sendNotification(const std::string& db) {
    DBConnection conn(db);
    std::ostringstream q;
    q << "INSERT INTO notifications(appointment_id, message) VALUES("
      << appointmentId << ", '" << message << "');";
    return conn.execute(q.str());
}

void Notification::runReminders(const std::string& db) {
    DBConnection conn(db);

    std::string q =
        "SELECT id, appointment_id, message FROM notifications "
        "WHERE createdAt IS NULL;";

    conn.query(q,
        [](void*, int, char**, char**) {
            // Normally: send SMS/email — here we just simulate.
            return 0;
        },
        nullptr
    );
}