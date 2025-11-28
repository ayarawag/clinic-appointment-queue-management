#include "../models/appointment.h"
#include "../database/db_connection.h"
#include <iostream>
using namespace std;

void rescheduleAppointment() {
    int id; string newTime;
    cout << "Appointment ID: "; cin >> id;
    cin.ignore();
    cout << "New Time (YYYY-MM-DD HH:MM): "; getline(cin, newTime);

    DBConnection db("clinic.db");
    int docId = 0;
    db.query("SELECT doctorId FROM appointments WHERE id=" + to_string(id) + ";", [](void* ud, int cols, char** vals, char**) -> int {
        int* p = (int*)ud;
        if (vals[0]) *p = stoi(vals[0]);
        return 0;
    }, &docId);

    Appointment a; a.id = id; a.doctorId = docId;
    if (a.reschedule(newTime)) cout << "Rescheduled successfully.\n";
    else cout << "Reschedule failed.\n";
}