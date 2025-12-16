#include "notification.h"
#include "../database/db_connection.h"
#include <iostream>
#include <sstream>

Notification::Notification() : patientId(0), message("") {}

Notification::Notification(int aid, std::string msg) {
    patientId = aid;
    message = msg;
}

bool Notification::sendNotification(const std::string& db) {
    DBConnection conn(db);

    std::ostringstream q;
    q << "INSERT INTO notifications(patientId, message) VALUES("
      << patientId << ",'" << message << "');";

    return conn.execute(q.str());
}

void Notification::runReminders(const std::string& db) {
    DBConnection conn(db);

    // Placeholder: just run a simple SELECT
    conn.query(
        "SELECT id FROM notifications;",
        [](void*, int, char**, char**) -> int { return 0; },
        nullptr
    );

    std::cout << "Reminders processed.\n";
}