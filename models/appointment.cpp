#include "appointment.h"
#include "../database/db_connection.h"
#include <sstream>
#include <string>
#include <cstdlib>

Appointment::Appointment() {
    id = 0;
    patientId = 0;
    doctorId = 0;
    status = "Scheduled";
}

bool Appointment::book(const std::string& db) {
    DBConnection x(db);
    std::ostringstream q;

    q << "INSERT INTO appointments(patient_id, doctor_id, date_time, status) VALUES("
      << patientId << "," << doctorId << ",'"
      << dateTime << "','" << status << "');";

    return x.execute(q.str());
}

bool Appointment::cancel(const std::string& db) {
    if (id == 0) return false;

    DBConnection x(db);
    std::ostringstream q;

    q << "UPDATE appointments SET status='Cancelled' WHERE id=" << id;

    return x.execute(q.str());
}

bool Appointment::reschedule(const std::string& newTime, const std::string& db) {
    if (id == 0) return false;

    DBConnection x(db);
    std::ostringstream q;

    q << "UPDATE appointments SET date_time='" << newTime
      << "', status='Rescheduled' WHERE id=" << id;

    return x.execute(q.str());
}

Appointment Appointment::loadById(int aid, const std::string& db) {
    Appointment a;
    DBConnection x(db);

    std::string q =
        "SELECT id, patient_id, doctor_id, date_time, status FROM appointments WHERE id = "
        + std::to_string(aid);

    x.query(
        q,
        [](void* out, int cols, char** vals, char**) -> int {
            Appointment* a = (Appointment*)out;

            if (vals[0]) a->id = std::stoi(vals[0]);
            if (vals[1]) a->patientId = std::stoi(vals[1]);
            if (vals[2]) a->doctorId = std::stoi(vals[2]);
            if (vals[3]) a->dateTime = vals[3];
            if (vals[4]) a->status = vals[4];

            return 0;
        },
        &a
    );

    return a;
}