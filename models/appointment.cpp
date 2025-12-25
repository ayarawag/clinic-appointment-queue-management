#include "appointment.h"
#include "../database/db_connection.h" 
#include <sstream>
#include <string>
#include <stdexcept> 
#include <iostream>
#include <vector>   // ⭐ جديد (Feature البحث)

// المُنشئات
Appointment::Appointment()
    : id(0), patientId(0), doctorId(0), dateTime(""), status(""), paid(0) {}

Appointment::Appointment(int pid, int did, std::string dt)
{
    id = 0;
    patientId = pid;
    doctorId = did;
    dateTime = dt;
    status = "Booked"; // الحالة الافتراضية
    paid = 0;
}

// ==========================================================
// 1. الدالة book
// ==========================================================
bool Appointment::book(const std::string& db)
{
    // Validation
    if (patientId <= 0 ||  doctorId <= 0 ||  dateTime.empty()) {
        std::cerr << "ERROR (Validation): Invalid booking data.\n";
        return false;
    }

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "INSERT INTO appointments(patientId, doctorId, dateTime, status, paid) VALUES("
          << patientId << "," << doctorId << ",'"
          << dateTime << "', 'Booked', 0);";

        bool success = conn->execute(q.str());

        if (success) {
            conn->query(
                "SELECT last_insert_rowid();",
                [](void* data, int, char** argv, char**) -> int {
                    if (argv[0]) {
                        *((int*)data) = std::stoi(argv[0]);
                    }
                    return 0;
                },
                &this->id
            );
        }

        return success;

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION (Book): " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 2. الدالة cancel
// ==========================================================
bool Appointment::cancel(const std::string& db)
{
    if (id == 0) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "UPDATE appointments SET status='Cancelled' WHERE id=" << id;

        bool ok = conn->execute(q.str());
        if (ok) status = "Cancelled";

        return ok;

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION (Cancel): " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 3. الدالة setPaid
// ==========================================================
bool Appointment::setPaid(bool paidValue, const std::string& db)
{
    if (id == 0) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "UPDATE appointments SET paid=" << (paidValue ? 1 : 0)
          << " WHERE id=" << id;

        bool ok = conn->execute(q.str());
        if (ok) paid = paidValue ? 1 : 0;

        return ok;

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION (SetPaid): " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 4. الدالة reschedule
// ==========================================================
bool Appointment::reschedule(const std::string& newDateTime, const std::string& db)
{
    if (id == 0) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "UPDATE appointments SET dateTime='" << newDateTime
          << "' WHERE id=" << id;

        bool ok = conn->execute(q.str());
        if (ok) dateTime = newDateTime;

        return ok;

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION (Reschedule): " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 5. الدالة loadById
// ==========================================================
Appointment Appointment::loadById(int aid, const std::string& db)
{
    Appointment a;

    try {DBConnection* conn = DBConnection::getInstance(db);

        std::string q =
            "SELECT id, patientId, doctorId, dateTime, status, paid "
            "FROM appointments WHERE id=" + std::to_string(aid) + " LIMIT 1;";

        conn->query(
            q,
            [](void* out, int, char** vals, char**) -> int {
                Appointment* a = static_cast<Appointment*>(out);
                if (vals[0]) a->id        = std::stoi(vals[0]);
                if (vals[1]) a->patientId = std::stoi(vals[1]);
                if (vals[2]) a->doctorId  = std::stoi(vals[2]);
                if (vals[3]) a->dateTime  = vals[3];
                if (vals[4]) a->status    = vals[4];
                if (vals[5]) a->paid      = std::stoi(vals[5]);
                return 0;
            },
            &a
        );

        return a;

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION (LoadById): " << e.what() << std::endl;
        return Appointment();
    }
}

// ==========================================================
// 6. الدالة search (Feature جديد - Assignment 7)
// ==========================================================
std::vector<Appointment> Appointment::search(
    int patientId,
    const std::string& date,
    int doctorId,
    const std::string& db
) {
    std::vector<Appointment> results;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "SELECT id, patientId, doctorId, dateTime, status, paid "
          << "FROM appointments WHERE patientId=" << patientId;

        if (!date.empty())
            q << " AND dateTime LIKE '" << date << "%'";

        if (doctorId > 0)
            q << " AND doctorId=" << doctorId;

        q << " ORDER BY dateTime;";

        conn->query(
            q.str(),
            [](void* out, int, char** vals, char**) -> int {
                auto* list = static_cast<std::vector<Appointment>*>(out);
                Appointment a;

                if (vals[0]) a.id        = std::stoi(vals[0]);
                if (vals[1]) a.patientId = std::stoi(vals[1]);
                if (vals[2]) a.doctorId  = std::stoi(vals[2]);
                if (vals[3]) a.dateTime  = vals[3];
                if (vals[4]) a.status    = vals[4];
                if (vals[5]) a.paid      = std::stoi(vals[5]);

                list->push_back(a);
                return 0;
            },
            &results
        );

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION (Search): " << e.what() << std::endl;
    }

    return results;
}