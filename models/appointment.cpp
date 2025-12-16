#include "appointment.h"
#include "../database/db_connection.h" 
#include <sstream>
#include <string>
#include <stdexcept> 
#include <iostream> 

// المُنشئات والهادمات
Appointment::Appointment()
    : id(0), patientId(0), doctorId(0), dateTime(""), status(""), paid(0) {}

Appointment::Appointment(int pid, int did, std::string dt)
{
    id = 0;
    patientId = pid;
    doctorId = did;
    dateTime = dt;
    status = "Booked"; // تعيين الحالة الافتراضية عند الإنشاء
    paid = 0;
}

// ==========================================================
// 1. الدالة book: تطبيق Singleton و Try/Catch
// ==========================================================
bool Appointment::book(const std::string& db)
{
    // التحقق من المدخلات (Validation)
    if (patientId <= 0 || doctorId <= 0 || dateTime.empty()) {
        std::cerr << "ERROR (Validation): Booking failed due to invalid IDs or empty date/time.\n";
        return false;
    }

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "INSERT INTO appointments(patientId, doctorId, dateTime, status, paid) VALUES("
          << patientId << "," << doctorId << ",'"
          << dateTime << "', 'Booked', 0);"; // [تعديل]: إضافة status

        bool success = conn->execute(q.str()); 

        if (success) {
            conn->query( 
                "SELECT last_insert_rowid();",
                [](void* data, int argc, char** argv, char** col_names) -> int {
                    if (argv[0]) {
                        try {
                            *((int*)data) = std::stoi(argv[0]); 
                        } catch (const std::exception& e) {
                            std::cerr << "Error parsing ID: " << e.what() << std::endl;
                        }
                    }
                    return 0;
                },
                &this->id
            );
        }
        
        return success;

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (Book): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 2. الدالة cancel: تحديث status بدلاً من الحذف (الحل للاختبارات)
// ==========================================================
bool Appointment::cancel(const std::string& db)
{
    if (id == 0) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "UPDATE appointments SET status='Cancelled' WHERE id=" << id;
        
        bool ok = conn->execute(q.str()); 
        
        if (ok) {
            this->status = "Cancelled"; // تحديث الكائن محلياً
        }
        return ok; 

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (Cancel): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 3. الدالة setPaid: تطبيق Singleton و Try/Catch
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
        std::cerr << "EXCEPTION CAUGHT (SetPaid): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 4. الدالة reschedule: تطبيق Singleton و Try/Catch
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
        std::cerr << "EXCEPTION CAUGHT (Reschedule): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 5. الدالة loadById: تطبيق Singleton و Try/Catch
// ==========================================================
Appointment Appointment::loadById(int aid, const std::string& db)
{
    Appointment a;
    
    try {
        DBConnection* conn = DBConnection::getInstance(db);

        // [تعديل الاستعلام]: إضافة status
        std::string q =
            "SELECT id, patientId, doctorId, dateTime, status, paid " 
            "FROM appointments WHERE id=" + std::to_string(aid) + " LIMIT 1;";

        conn->query(
           q,
            [](void* out, int, char** vals, char**) -> int {
                Appointment* a = (Appointment*)out;
                if (vals[0]) a->id        = std::stoi(vals[0]);
                if (vals[1]) a->patientId = std::stoi(vals[1]);
                if (vals[2]) a->doctorId  = std::stoi(vals[2]);
                if (vals[3]) a->dateTime  = vals[3];
                if (vals[4]) a->status    = vals[4]; // <-- تعبئة status
                if (vals[5]) a->paid      = std::stoi(vals[5]);
                return 0;
            },
            &a
        );

        return a;

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (LoadById): DB query failed: " << e.what() << std::endl;
        return Appointment(); 
    }
}