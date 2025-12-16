#include "db_connection.h"
#include <iostream>

// [إضافة 1] تهيئة المؤشر الثابت (يجب أن يكون nullptr خارج الكلاس)
DBConnection* DBConnection::instance = nullptr; 


// [تعديل] أصبح المُنشئ الآن خاصاً (private)
DBConnection::DBConnection(const std::string& filename) {
    if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open DB: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

// [تعديل] أصبح الهادم الآن خاصاً (private)
DBConnection::~DBConnection() {
    if (db) sqlite3_close(db);
}

// [إضافة 2] تنفيذ دالة الوصول (getInstance)
DBConnection* DBConnection::getInstance(const std::string& filename) {
    // التحقق مما إذا كانت النسخة موجودة بالفعل
    if (instance == nullptr) {
        // إذا لم تكن موجودة، قم بإنشاء نسخة جديدة
        // يتم استخدام المُنشئ الخاص هنا
        instance = new DBConnection(filename);
    }
    // إرجاع النسخة الموجودة (أو التي تم إنشاؤها للتو)
    return instance;
}

// [إضافة 3 اختيارية] تنفيذ دالة تدمير النسخة للتنظيف
void DBConnection::destroyInstance() {
    if (instance != nullptr) {
        delete instance; // استدعاء الهادم الخاص (~DBConnection)
        instance = nullptr;
    }
}


// تبقى الدالتان execute و query كما هما:
bool DBConnection::execute(const std::string& query) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    // ... باقي الكود ...
    if (rc != SQLITE_OK) {
        std::cerr << "DB Error: " << (errMsg ? errMsg : "Unknown") << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DBConnection::query(const std::string& query,
                         int (*callback)(void*, int, char**, char**),
                         void* data) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), callback, data, &errMsg);
    // ... باقي الكود ...
    if (rc != SQLITE_OK) {
        std::cerr << "DB Query Error: " << (errMsg ? errMsg : "Unknown") << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}