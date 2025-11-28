#include "../database/db_connection.h"
#include <iostream>
using namespace std;

void sendNotification(int patientId, const string& message) {
    DBConnection db("clinic.db");
    string channel = "app";
    db.query("SELECT preferredChannel FROM patients WHERE id=" + to_string(patientId) + ";",
        [](void* ud, int cols, char** vals, char**) -> int {
            string* s = (string*)ud;
            if (vals[0]) *s = vals[0];
            return 0;
        }, &channel);

    cout << "Notify (via " << channel << ") patient " << patientId << " : " << message << "\n";
}

void runReminders(int minutesBefore) {
    DBConnection db("clinic.db");
    cout << "Reminder: (simple demo) listing all appointments:\n";
    db.query("SELECT id, patientId, dateTime FROM appointments;", [](void* ud, int cols, char** vals, char**) -> int {
        if (vals[0] && vals[1] && vals[2]) {
            cout << "Appointment " << vals[0] << " | patient " << vals[1] << " | at " << vals[2] << "\n";
        }
        return 0;
    }, nullptr);
}