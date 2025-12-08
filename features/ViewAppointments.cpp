#include "../database/db_connection.h"
#include <iostream>

void viewAppointmentsCLI() {
    DBConnection db("clinic.db");
    db.query("SELECT id, patientId, doctorId, dateTime, status, paid FROM appointments ORDER BY dateTime ASC;",
        [](void*, int cols, char** vals, char**) -> int {
            std::cout << "ID:" << (vals[0]?vals[0]:"") << " | PatientID:" << (vals[1]?vals[1]:"")
                      << " | DoctorID:" << (vals[2]?vals[2]:"") << " | Time:" << (vals[3]?vals[3]:"")
                      << " | Status:" << (vals[4]?vals[4]:"") << " | Paid:" << (vals[5]?vals[5]:"0") << "\n";
            return 0;
        }, nullptr);
}