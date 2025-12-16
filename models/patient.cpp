#include "patient.h"
#include "../database/db_connection.h"
#include "../utils/password_utils.h"
#include "../utils/time_utils.h"
#include <sstream>
#include <string>
#include <cstdlib>
#include <iostream> 
#include <stdexcept> // لإضافة std::stoi

using std::stoi;

Patient::Patient() : id(0) {}

Patient::Patient(std::string n, std::string p, std::string e, std::string pass) {
    id = 0;
    name = n;
    phone = p;
    email = e;
    passwordHash = PasswordUtils::hashPassword(pass);
}

bool Patient::emailExists(std::string email, const std::string& dbfile) {
    DBConnection db(dbfile);
    bool found = false;

    db.query(
        "SELECT id FROM patients WHERE email='" + email + "' LIMIT 1;",
        [](void* u, int c, char** v, char**) {
            if (v[0]) *(bool*)u = true;
            return 0;
        },
        &found
    );

    return found;
}

Patient Patient::loadById(int pid, const std::string& db) {
    Patient p;
    DBConnection dbx(db);

    dbx.query(
        "SELECT id,name,phone,email,password_hash FROM patients WHERE id=" + std::to_string(pid),
        [](void* u, int c, char** v, char**) {
            Patient* p = (Patient*)u;

            if (v[0]) p->id = stoi(v[0]);
            if (v[1]) p->name = v[1];
            if (v[2]) p->phone = v[2];
            if (v[3]) p->email = v[3];
            if (v[4]) p->passwordHash = v[4];

            return 0;
        },
        &p
    );

    return p;
}

bool Patient::registerPatient(const std::string& db) {
    // 1. التحقق من البيانات (Validation)
    if (name.empty() || phone.empty() || email.empty() || passwordHash.empty()) {
        std::cerr << "ERROR: Registration failed due to missing required data (Validation).\n";
        return false;
    }
    
    // 2. التحقق من التكرار
    if (emailExists(email, db)) return false;

    DBConnection x(db);
    std::ostringstream q;

    q << "INSERT INTO patients(name, phone, email, password_hash, createdAt) VALUES('"
      << name << "','"
      << phone << "','"
      << email << "','"
      << passwordHash << "',"
      << TimeUtils::nowEpochSeconds() << ");";

    bool success = x.execute(q.str());
    
    // 3. التعديل الجديد: استرجاع الـ ID الذي تم إنشاؤه حديثاً
    if (success) {
        // استرجاع الـ ID الأخير الذي تم إدخاله (SQLite-specific)
        x.query(
            "SELECT last_insert_rowid();",
            [](void* data, int argc, char** argv, char** col_names) -> int {
                if (argv[0]) {
                    // تعيين الـ ID للكائن الحالي (this->id)
                    // يجب استخدام try-catch إذا كان هناك خوف من فشل stoi، ولكن نفترض النجاح هنا.
                    try {
                        *((int*)data) = std::stoi(argv[0]); 
                    } catch (const std::exception& e) {
                        // طباعة خطأ بسيط إذا فشل التحويل (اختياري)
                        std::cerr << "ERROR: Failed to convert last row ID to integer: " << e.what() << std::endl;
                    }
                }
                return 0;
            },
            &this->id // تمرير عنوان الخاصية id للكائن الحالي
        );
    }
    
    return success;
}

bool Patient::update(const std::string& db) {
    if (id == 0) return false;

    DBConnection x(db);
    std::ostringstream q;

    q << "UPDATE patients SET name='" << name
      << "', phone='" << phone
      << "', email='" << email
      << "' WHERE id=" << id;

    return x.execute(q.str());
}

bool Patient::remove(const std::string& db) {
    if (id == 0) return false;

    DBConnection x(db);
    return x.execute("DELETE FROM patients WHERE id=" + std::to_string(id));
}