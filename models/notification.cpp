#include "notification.h"
#include "../database/db_connection.h"
#include <sstream>
#include <iostream>

bool Notification::sendNotification(int pid, const std::string& msg, const std::string& dbfile) {
    DBConnection db(dbfile);
    std::ostringstream q;
    q << "INSERT INTO notifications(patient_id, message) VALUES("
      << pid << ", '" << msg << "');";
    return db.execute(q.str());
}

void Notification::runReminders(int minutesBefore, const std::string& dbfile) {
    DBConnection db(dbfile);
    std::string q =
        "SELECT id, patient_id, date_time "
        "FROM appointments WHERE status='Scheduled';";

    db.query(q,
        [](void* ud, int c, char** v, char**) -> int {
            int pid = std::stoi(v[1]);
            std::string msg = "Reminder: Your appointment is coming soon.";
            Notification::sendNotification(pid, msg);
            return 0;
        },
        nullptr);
}