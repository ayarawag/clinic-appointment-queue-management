#include "doctor.h"
#include "../database/db_connection.h" 
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <vector>   //  جديد
#include <algorithm>

Doctor::Doctor() : id(0), name(""), specialization(""), schedule("") {}

Doctor::Doctor(int i, std::string n, std::string s) {
    id = i;
    name = n;
    specialization = s;
    schedule = "";
}

Doctor::Doctor(std::string n, std::string s) {
    id = 0;
    name = n;
    specialization = s;
    schedule = "";
}

// ==========================================================
// 1. الدالة registerDoctor: تطبيق Singleton و Try/Catch
// ==========================================================
bool Doctor::registerDoctor(const std::string& db) {
    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "INSERT INTO doctors(name, specialization) VALUES('"
          << name << "','" << specialization<< "');";

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
        std::cerr << "EXCEPTION CAUGHT (Register): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 2. الدالة loadById: تطبيق Singleton و Try/Catch
// ==========================================================
Doctor Doctor::loadById(int did, const std::string& db) {
    Doctor d;
    
    try {
        DBConnection* conn = DBConnection::getInstance(db);

        std::string q =
            "SELECT id,name,specialization,schedule FROM doctors WHERE id=" + std::to_string(did);

        conn->query(
            q,
            [](void* out, int, char** vals, char**) -> int {
                Doctor* d = (Doctor*)out;

                if (vals[0]) d->id = std::stoi(vals[0]);
                if (vals[1]) d->name = vals[1];
                if (vals[2]) d->specialization = vals[2];
                if (vals[3]) d->schedule = vals[3];

                return 0;
            },
            &d);

        return d;

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (LoadById): DB query failed: " << e.what() << std::endl;
        return Doctor(); 
    }
}

// ==========================================================
// 3. الدالة setSchedule: تطبيق Singleton و Try/Catch
// ==========================================================
bool Doctor::setSchedule(const std::string& sched, const std::string& db) {
    if (id == 0) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "UPDATE doctors SET schedule='" << sched << "' WHERE id=" << id;

        return conn->execute(q.str());
    
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (SetSchedule): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 4. الدالة update: تطبيق Singleton و Try/Catch
// ==========================================================
bool Doctor::update(const std::string& db) {
    if (id == 0) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "UPDATE doctors SET name='" << name
          << "', specialization='" << specialization
          << "' WHERE id=" << id;

        return conn->execute(q.str());
    
    } catch (const std::exception& e) {std::cerr << "EXCEPTION CAUGHT (Update): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 5. الدالة remove: تطبيق Singleton و Try/Catch
// ==========================================================
bool Doctor::remove(const std::string& db) {
    if (id == 0) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "DELETE FROM doctors WHERE id=" << id;

        return conn->execute(q.str());
    
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (Remove): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// ⭐⭐⭐ 6. Feature 11: View Available Appointment Slots
// ==========================================================
std::vector<std::string> Doctor::getAvailableSlots(
    int doctorId,
    const std::string& date,
    const std::string& db
) {
    std::vector<std::string> slots;

    try {
        DBConnection* conn = DBConnection::getInstance(db);

        // مثال بسيط: نعتبر كل ساعة من 09:00 إلى 13:00
        std::vector<std::string> allSlots = {
            "09:00", "10:00", "11:00", "12:00", "13:00"
        };

        std::vector<std::string> bookedSlots;

        std::string q =
            "SELECT substr(dateTime, 12, 5) FROM appointments "
            "WHERE doctorId=" + std::to_string(doctorId) +
            " AND dateTime LIKE '" + date + "%' "
            "AND status='Booked';";

        conn->query(
            q,
            [](void* out, int, char** vals, char**) -> int {
                if (vals[0]) {
                    ((std::vector<std::string>*)out)->push_back(vals[0]);
                }
                return 0;
            },
            &bookedSlots
        );

        // إرجاع الأوقات غير المحجوزة
        for (const auto& s : allSlots) {
            if (std::find(bookedSlots.begin(), bookedSlots.end(), s) == bookedSlots.end()) {
                slots.push_back(s);
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (AvailableSlots): " << e.what() << std::endl;
    }

    return slots;
}