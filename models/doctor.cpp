#include "doctor.h"
#include "../database/db_connection.h"
#include <sstream>
#include <stdexcept> // لإضافة std::stoi

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

bool Doctor::registerDoctor(const std::string& db) {
    DBConnection conn(db);
    std::ostringstream q;

    q << "INSERT INTO doctors(name, specialization) VALUES('"
      << name << "','" << specialization<< "');";

    bool success = conn.execute(q.str()); // تخزين نتيجة التنفيذ

    // ==========================================================
    // التعديل الجديد: استرجاع الـ ID الذي تم إنشاؤه حديثاً
    // هذا يحل مشكلة فشل ASSERT_TRUE(doctorId != 0) في اختبارات BookingTests
    // ==========================================================
    if (success) {
        // استرجاع الـ ID الأخير الذي تم إدخاله (SQLite-specific)
        conn.query(
            "SELECT last_insert_rowid();",
            [](void* data, int argc, char** argv, char** col_names) -> int {
                if (argv[0]) {
                    // تعيين الـ ID للكائن الحالي (this->id)
                    try {
                        *((int*)data) = std::stoi(argv[0]); 
                    } catch (const std::exception& e) {
                        // التعامل مع خطأ التحويل إذا حدث (اختياري)
                    }
                }
                return 0;
            },
            &this->id // تمرير عنوان الخاصية id للكائن الحالي
        );
    }
    
    return success;
    // ==========================================================
}

Doctor Doctor::loadById(int did, const std::string& db) {
    Doctor d;
    DBConnection conn(db);

    std::string q =
        "SELECT id,name,specialization,schedule FROM doctors WHERE id=" + std::to_string(did);

    conn.query(
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
}

bool Doctor::setSchedule(const std::string& sched, const std::string& db) {
    if (id == 0) return false;

    DBConnection conn(db);
    std::ostringstream q;

    q << "UPDATE doctors SET schedule='" << sched << "' WHERE id=" << id;

    return conn.execute(q.str());
}

// NEW — REQUIRED BY TESTS
bool Doctor::update(const std::string& db) {
    if (id == 0) return false;

    DBConnection conn(db);
    std::ostringstream q;

    q << "UPDATE doctors SET name='" << name
      << "', specialization='" << specialization
      << "' WHERE id=" << id;

    return conn.execute(q.str());
}

// NEW — REQUIRED BY TESTS
bool Doctor::remove(const std::string& db) {
    if (id == 0) return false;

    DBConnection conn(db);
    std::ostringstream q;

    q << "DELETE FROM doctors WHERE id=" << id;

    return conn.execute(q.str());
}