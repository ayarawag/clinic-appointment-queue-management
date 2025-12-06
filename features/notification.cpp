#include "../database/db_connection.h"
#include "../models/notification.h" // يحتوي الآن على تعريفات الدوال sendNotification و runReminders
#include <iostream>
#include <string>

using namespace std;

// Function to send a notification to a patient via their preferred channel
void sendNotification(int patientId, const string& message) {
    DBConnection db("clinic.db");
    string channel = "app";
    
    // Retrieve preferred channel from the patients table
    db.query("SELECT preferredChannel FROM patients WHERE id=" + to_string(patientId) + ";",
        [](void* ud, int cols, char** vals, char**) -> int {
            string* s = (string*)ud;
            if (vals[0]) *s = vals[0];
            return 0;
        }, &channel);

    cout << "Notify (via " << channel << ") patient " << patientId << " : " << message << "\n";
}

// Function to run appointment reminders (simple demo listing appointments)
void runReminders(int minutesBefore) {
    DBConnection db("clinic.db");
    cout << "Reminder: (simple demo) listing all appointments:\n";
    
    // Retrieve list of appointments
    db.query("SELECT id, patientId, dateTime FROM appointments;", 
        [](void* ud, int cols, char** vals, char**) -> int {
        if (vals[0] && vals[1] && vals[2]) {
            // Display appointments
            cout << "Appointment " << vals[0] << " | patient " << vals[1] << " | at " << vals[2] << "\n";
        }
        return 0;
    }, nullptr);
}