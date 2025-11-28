#include "../database/db_connection.h"
#include <iostream>
using namespace std;

void viewAppointments() {
    DBConnection db("clinic.db");
    db.query("SELECT a.id, p.name, d.name, a.dateTime, a.paid FROM appointments a LEFT JOIN patients p ON a.patientId=p.id LEFT JOIN doctors d ON a.doctorId=d.id;",
        [](void*, int cols, char** vals, char**) -> int {
            cout << "ID:" << (vals[0]?vals[0]:"") << " | Patient:" << (vals[1]?vals[1]:"") << " | Doctor:" << (vals[2]?vals[2]:"")
                 << " | Time:" << (vals[3]?vals[3]:"") << " | Paid:" << (vals[4]?vals[4]:"0") << "\n";
            return 0;
        }, nullptr);
    }