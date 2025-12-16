#include "appointment.h"
#include "../database/db_connection.h"
#include <sstream>
#include <string>

Appointment::Appointment()
    : id(0), patientId(0), doctorId(0), dateTime(""), paid(0) {}

Appointment::Appointment(int pid, int did, std::string dt)
{
    id = 0;
    patientId = pid;
    doctorId = did;
    dateTime = dt;
    paid = 0;
}

bool Appointment::book(const std::string& db)
{
    DBConnection conn(db);
    std::ostringstream q;

    q << "INSERT INTO appointments(patientId, doctorId, dateTime, paid) VALUES("
      << patientId << "," << doctorId << ",'"
      << dateTime << "',0);";

    return conn.execute(q.str());
}

bool Appointment::cancel(const std::string& db)
{
    if (id == 0) return false;

    DBConnection conn(db);
    std::ostringstream q;

    q << "DELETE FROM appointments WHERE id=" << id;
    return conn.execute(q.str());
}

bool Appointment::setPaid(bool paidValue, const std::string& db)
{
    if (id == 0) return false;

    DBConnection conn(db);
    std::ostringstream q;

    q << "UPDATE appointments SET paid=" << (paidValue ? 1 : 0)
      << " WHERE id=" << id;

    bool ok = conn.execute(q.str());
    if (ok) paid = paidValue ? 1 : 0;
    return ok;
}

bool Appointment::reschedule(const std::string& newDateTime, const std::string& db)
{
    if (id == 0) return false;

    DBConnection conn(db);
    std::ostringstream q;

    q << "UPDATE appointments SET dateTime='" << newDateTime
      << "' WHERE id=" << id;

    bool ok = conn.execute(q.str());
    if (ok) dateTime = newDateTime;

    return ok;
}

Appointment Appointment::loadById(int aid, const std::string& db)
{
    Appointment a;
    DBConnection conn(db);

    std::string q =
        "SELECT id, patientId, doctorId, dateTime, paid "
        "FROM appointments WHERE id=" + std::to_string(aid) + " LIMIT 1;";

    conn.query(
        q,
        [](void* out, int, char** vals, char**) -> int {
            Appointment* a = (Appointment*)out;
            if (vals[0]) a->id        = std::stoi(vals[0]);
            if (vals[1]) a->patientId = std::stoi(vals[1]);
            if (vals[2]) a->doctorId  = std::stoi(vals[2]);
            if (vals[3]) a->dateTime  = vals[3];
            if (vals[4]) a->paid      = std::stoi(vals[4]);
            return 0;
        },
        &a
    );

    return a;
}