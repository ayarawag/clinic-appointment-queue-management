#include "appointment.h"
#include "../database/db_connection.h"
#include <iostream>
#include <string>
using namespace std;

bool Appointment::book() {
    DBConnection db("../clinic.db"); 
    if (!db.execute("BEGIN TRANSACTION;")) return false;

    // 1. التحقق من التوفر
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

    // 2. إدراج الموعد
    string insert = "INSERT INTO appointments(patientId,doctorId,dateTime,paid) VALUES(" + to_string(patientId) + "," + to_string(doctorId) + ",'" + dateTime + "',0);";
    if (!db.execute(insert)) { 
        db.execute("ROLLBACK;"); 
        return false; 
    }
    
    // 3. جلب الـ ID
    db.query("SELECT last_insert_rowid();", [](void* ud, int cols, char** vals, char**) -> int {
        int* p = (int*)ud;
        if (vals[0]) *p = stoi(vals[0]);
        return 0;
    }, &id);
    
    paid = false;

    db.execute("COMMIT;");
    cout << "Appointment booked successfully with ID: " << id << endl;
    return true;
}

bool Appointment::cancel() {
    DBConnection db("../clinic.db"); 
    
    // جلب patientId
    int pid = 0;
    db.query("SELECT patientId FROM appointments WHERE id=" + to_string(id) + ";", [](void* ud, int cols, char** vals, char**) -> int {
        int* p = (int*)ud;
        if (vals[0]) *p = stoi(vals[0]);
        return 0;
    }, &pid);

    if (!db.execute("DELETE FROM appointments WHERE id=" + to_string(id) + ";")) return false;

    // الإزالة من جدول الانتظار (queue) إذا كان موجوداً
    if (pid > 0) db.execute("DELETE FROM queue WHERE patientId=" + to_string(pid) + ";");
    
    cout << "Appointment ID " << id << " cancelled.\n";
    return true;
}

bool Appointment::reschedule(const string& newTime) {
    DBConnection db("../clinic.db"); 
    if (!db.execute("BEGIN TRANSACTION;")) return false;

    // جلب doctorId
    int did = 0;
    db.query("SELECT doctorId FROM appointments WHERE id=" + to_string(id) + ";", [](void* ud, int cols, char** vals, char**) -> int {
        int* p = (int*)ud;
        if (vals[0]) *p = stoi(vals[0]);
        return 0;
    }, &did);

    // التحقق من توفر الفتحة
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

    // التحديث
    if (!db.execute("UPDATE appointments SET dateTime='" + newTime + "' WHERE id=" + to_string(id) + ";")) { 
        db.execute("ROLLBACK;"); 
        return false; 
    }
    
    dateTime = newTime;
    db.execute("COMMIT;");
    cout << "Appointment ID " << id << " rescheduled to " << newTime << ".\n";
    return true;
}

bool Appointment::setPaid() {
    DBConnection db("../clinic.db"); 
    if (db.execute("UPDATE appointments SET paid=1 WHERE id=" + to_string(id) + ";")) {
        paid = true;
        cout << "Appointment ID " << id << " marked as paid.\n";
        return true;
    }
    return false;
}

bool Appointment::getAppointmentById(int appointmentId) {
    DBConnection db("../clinic.db");
    id = appointmentId;

    string sql = "SELECT patientId, doctorId, dateTime, paid FROM appointments WHERE id=" + to_string(id) + ";";

    struct AppointmentData { Appointment* a; } data = {this};

    db.query(sql,
        [](void* data, int cols, char** vals, char**) -> int {
            AppointmentData* d = (AppointmentData*)data;
            if (vals[0]) d->a->patientId = stoi(vals[0]);
            if (vals[1]) d->a->doctorId = stoi(vals[1]);
            if (vals[2]) d->a->dateTime = vals[2];
            if (vals[3]) d->a->paid = (stoi(vals[3]) == 1);
            return 0;
        }, &data
    );
    
    // التحقق من وجود الموعد
    return patientId != 0;
}