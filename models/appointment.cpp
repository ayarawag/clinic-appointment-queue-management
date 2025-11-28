#include "appointment.h"
#include "../database/db_connection.h"
#include <iostream>
using namespace std;

bool Appointment::book() {
    DBConnection db("clinic.db");
    if (!db.execute("BEGIN TRANSACTION;")) return false;

    string check = "SELECT COUNT(*) FROM appointments WHERE doctorId=" + to_string(doctorId) + " AND dateTime='" + dateTime + "';";
    int cnt = 0;
    db.query(check, [](void* ud, int cols, char** vals, char**) -> int {
        int* p = (int*)ud;
        if (vals[0]) *p = stoi(vals[0]);
        return 0;
    }, &cnt);

    if (cnt > 0) {
        cout << "Slot already taken.\n";
        db.execute("ROLLBACK;");
        return false;
    }

    string insert = "INSERT INTO appointments(patientId,doctorId,dateTime,paid) VALUES(" + to_string(patientId) + "," + to_string(doctorId) + ",'" + dateTime + "',0);";
    if (!db.execute(insert)) { db.execute("ROLLBACK;"); return false; }

    db.execute("COMMIT;");
    return true;
}

bool Appointment::cancel() {
    DBConnection db("clinic.db");
    // get patientId for queue removal
    int pid = 0;
    db.query("SELECT patientId FROM appointments WHERE id=" + to_string(id) + ";", [](void* ud, int cols, char** vals, char**) -> int {
        int* p = (int*)ud;
        if (vals[0]) *p = stoi(vals[0]);
        return 0;
    }, &pid);

    if (!db.execute("DELETE FROM appointments WHERE id=" + to_string(id) + ";")) return false;

    // remove from queue table if present
    if (pid > 0) db.execute("DELETE FROM queue WHERE patientId=" + to_string(pid) + ";");

    // TODO: refresh positions (simple approach later)
    return true;
}

bool Appointment::reschedule(const string& newTime) {
    DBConnection db("clinic.db");
    if (!db.execute("BEGIN TRANSACTION;")) return false;

    // find doctorId for this appointment
    int did = 0;
    db.query("SELECT doctorId FROM appointments WHERE id=" + to_string(id) + ";", [](void* ud, int cols, char** vals, char**) -> int {
        int* p = (int*)ud;
        if (vals[0]) *p = stoi(vals[0]);
        return 0;
    }, &did);

    string check = "SELECT COUNT(*) FROM appointments WHERE doctorId=" + to_string(did) + " AND dateTime='" + newTime + "' AND id!=" + to_string(id) + ";";
    int cnt = 0;
    db.query(check, [](void* ud, int cols, char** vals, char**) -> int {
        int* p = (int*)ud;
        if (vals[0]) *p = stoi(vals[0]);
        return 0;
    }, &cnt);

    if (cnt > 0) {
        cout << "Target slot already taken.\n";
        db.execute("ROLLBACK;");
        return false;
    }

    if (!db.execute("UPDATE appointments SET dateTime='" + newTime + "' WHERE id=" + to_string(id) + ";")) { db.execute("ROLLBACK;"); return false; }
    db.execute("COMMIT;");
    return true;
}

bool Appointment::setPaid() {
    DBConnection db("clinic.db");
    return db.execute("UPDATE appointments SET paid=1 WHERE id=" + to_string(id) + ";");
}