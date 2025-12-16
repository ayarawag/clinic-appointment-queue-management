#include "patient.h"
#include "../database/db_connection.h" 
#include "../utils/password_utils.h"
#include "../utils/time_utils.h"
#include <sstream>
#include <string>
#include <cstdlib>
#include <iostream> 
#include <stdexcept> 

using std::stoi;
using std::string; 

Patient::Patient() : id(0) {}

Patient::Patient(string n, string p, string e, string pass) {
    id = 0;
    name = n;
    phone = p;
    email = e;
    passwordHash = PasswordUtils::hashPassword(pass);
}

// ==========================================================
// 1. الدالة emailExists: تطبيق Singleton و Try/Catch
// ==========================================================
bool Patient::emailExists(string email, const string& dbfile) {
    bool found = false;

    try {
        DBConnection* conn = DBConnection::getInstance(dbfile);

        conn->query(
            "SELECT id FROM patients WHERE email='" + email + "' LIMIT 1;",
            [](void* u, int c, char** v, char**) {
                if (v[0]) *(bool*)u = true;
                return 0;
            },
            &found
        );
        return found;
        
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (EmailExists): DB query failed: " << e.what() << std::endl;
        return false; 
    }
}

// ==========================================================
// 2. الدالة loadById: تطبيق Singleton و Try/Catch
// ==========================================================
Patient Patient::loadById(int pid, const string& db) {
    Patient p;
    
    try {
        DBConnection* conn = DBConnection::getInstance(db);

        conn->query(
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

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (LoadById): DB query failed: " << e.what() << std::endl;
        return Patient();
    }
}

// ==========================================================
// 3. الدالة registerPatient: تطبيق Singleton و Try/Catch
// ==========================================================
bool Patient::registerPatient(const string& db) {
    // 1. التحقق من البيانات (Validation)
    if (name.empty() || phone.empty()||  email.empty() || passwordHash.empty()) {
        std::cerr << "ERROR: Registration failed due to missing required data (Validation).\n";
        return false;
    }
    
    // 2. التحقق من التكرار (هذه الدالة تستخدم Singleton الآن)
    if (emailExists(email, db)) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "INSERT INTO patients(name, phone, email, password_hash, createdAt) VALUES('"
          << name << "','"
          << phone << "','"
          << email << "','"
          << passwordHash << "',"
          << TimeUtils::nowEpochSeconds() << ");";

        bool success = conn->execute(q.str());
        
        // 3. استرجاع الـ ID الذي تم إنشاؤه حديثاً
        if (success) {
            conn->query(
                "SELECT last_insert_rowid();",
                [](void* data, int argc, char** argv, char** col_names) -> int {
                    if (argv[0]) {
                        try {
                            *((int*)data) = std::stoi(argv[0]); 
                        } catch (const std::exception& e) {
                            std::cerr << "ERROR: Failed to convert last row ID: " << e.what() << std::endl;
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
// 4. الدالة update: تطبيق Singleton و Try/Catch
// ==========================================================
bool Patient::update(const string& db) {
    if (id == 0) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "UPDATE patients SET name='" << name
          << "', phone='" << phone
          << "', email='" << email
          << "' WHERE id=" << id;

        return conn->execute(q.str());
        
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (Update): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 5. الدالة remove: تطبيق Singleton و Try/Catch
// ==========================================================
bool Patient::remove(const string& db) {
    if (id == 0) return false;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        
        return conn->execute("DELETE FROM patients WHERE id=" + std::to_string(id));
        
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (Remove): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}