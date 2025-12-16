#include "doctor.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <sstream>
#include <stdexcept>
#include <iostream> // للتصحيح وعرض الأخطاء

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
        // [Singleton] استخدام getInstance للحصول على نسخة الاتصال الوحيدة
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "INSERT INTO doctors(name, specialization) VALUES('"
          << name << "','" << specialization<< "');";

        // [Singleton] استخدام المؤشر -> لتنفيذ الاستعلام
        bool success = conn->execute(q.str()); 

        if (success) {
            // استرجاع الـ ID الذي تم إنشاؤه
            conn->query( // [Singleton] استخدام المؤشر ->
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
                &this->id // تمرير عنوان الخاصية id
            );
        }
        
        return success;

    } catch (const std::exception& e) {
        // [Try/Catch] معالجة أي استثناء
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
        // [Singleton] استخدام getInstance
        DBConnection* conn = DBConnection::getInstance(db);

        std::string q =
            "SELECT id,name,specialization,schedule FROM doctors WHERE id=" + std::to_string(did);

        // [Singleton] استخدام المؤشر ->
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
        // [Try/Catch] في حالة فشل التحميل، يتم إرجاع كائن Doctor فارغ (id=0)
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
        // [Singleton] استخدام getInstance
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "UPDATE doctors SET schedule='" << sched << "' WHERE id=" << id;

        // [Singleton] استخدام المؤشر ->
        return conn->execute(q.str());
    
    } catch (const std::exception& e) {
        // [Try/Catch]
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
        // [Singleton] استخدام getInstance
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "UPDATE doctors SET name='" << name
         << "', specialization='" << specialization
          << "' WHERE id=" << id;

        // [Singleton] استخدام المؤشر ->
        return conn->execute(q.str());
    
    } catch (const std::exception& e) {
        // [Try/Catch]
        std::cerr << "EXCEPTION CAUGHT (Update): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 5. الدالة remove: تطبيق Singleton و Try/Catch
// ==========================================================
bool Doctor::remove(const std::string& db) {
    if (id == 0) return false;

    try {
        // [Singleton] استخدام getInstance
        DBConnection* conn = DBConnection::getInstance(db);
        std::ostringstream q;

        q << "DELETE FROM doctors WHERE id=" << id;

        // [Singleton] استخدام المؤشر ->
        return conn->execute(q.str());
    
    } catch (const std::exception& e) {
        // [Try/Catch]
        std::cerr << "EXCEPTION CAUGHT (Remove): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}