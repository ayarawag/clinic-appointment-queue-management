#include "db_connection.h"
#include <iostream>
#include <stdexcept> // لإضافة معالجة الاستثناءات
DBConnection* DBConnection::instance = nullptr; 
// [تعديل Singleton] أصبح المُنشئ الآن خاصاً (private)
DBConnection::DBConnection(const std::string& filename) {
    if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        std::cerr << "CRITICAL ERROR: Failed to open DB: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
        // هنا يجب رمي استثناء إذا فشل الاتصال بشكل حرج
        throw std::runtime_error("Database connection failed.");
    }
}
DBConnection::~DBConnection() {
    if (db) {
        sqlite3_close(db);
        std::cout << "Database connection closed.\n"; // رسالة اختيارية
    }
}

// [تعديل Singleton] تنفيذ دالة الوصول (getInstance)
DBConnection* DBConnection::getInstance(const std::string& filename) {
    // التحقق مما إذا كانت النسخة موجودة بالفعل
    if (instance == nullptr) {
        // [Try/Catch] يجب تغليف الإنشاء لاحتمالية فشل فتح الملف
        try {
            // إذا لم تكن موجودة، قم بإنشاء نسخة جديدة
            instance = new DBConnection(filename);
        } catch (const std::exception& e) {
            std::cerr << "Failed" << e.what() << std::endl;
            // يجب أن نضمن أن المؤشر يبقى nullptr إذا فشل الإنشاء
            instance = nullptr;
        }
    }
    // إرجاع النسخة الموجودة
    return instance;
}

// [تعديل Singleton] تنفيذ دالة تدمير النسخة (الحل لمشكلة الاختبارات)
void DBConnection::destroyInstance() {
    if (instance != nullptr) {
        // Note: الهادم (~DBConnection) سيقوم بإغلاق sqlite3_close(db)
        delete instance; 
        instance = nullptr; // إعادة تعيين المؤشر إلى nullptr
        std::cout << "Singleton DB instance destroyed.\n"; // رسالة اختيارية
    }
}


// [تعديل Try/Catch] تنفيذ دالة execute
bool DBConnection::execute(const std::string& query) {
    // [الحماية] تحقق من أن الاتصال مفتوح
    if (!db) {
        std::cerr << "DB Error: Cannot execute query. Connection is closed or failed to open.\n";
        return false;
    }
    
    try {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
        
        if (rc != SQLITE_OK) {
            std::cerr << "DB Error: " << (errMsg ? errMsg : "Unknown") << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
        
    } catch (const std::exception& e) {
        // التقاط أي استثناء غير متوقع
        std::cerr << "EXCEPTION CAUGHT (DB Execute): " << e.what() << std::endl;
        return false;
    }
}

// [تعديل Try/Catch] تنفيذ دالة query
bool DBConnection::query(const std::string& query,
                         int (*callback)(void*, int, char**, char**),
                         void* data) {
    // [الحماية] تحقق من أن الاتصال مفتوح
    if (!db) {
        std::cerr << "DB Query Error: Cannot execute query. Connection is closed or failed to open.\n";
        return false;
    }
                         
    try {
        char* errMsg = nullptr;
        int rc = sqlite3_exec(db, query.c_str(), callback, data, &errMsg);
        
        if (rc != SQLITE_OK) {
            std::cerr << "DB Query Error: " << (errMsg ? errMsg : "Unknown") << std::endl;
            sqlite3_free(errMsg);
            return false;
        }
        return true;
        
    } catch (const std::exception& e) {
        // التقاط أي استثناء غير متوقع
        std::cerr << "EXCEPTION CAUGHT (DB Query): " << e.what() << std::endl;
        return false;
    }
}