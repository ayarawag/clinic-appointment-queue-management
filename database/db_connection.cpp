#include "db_connection.h"
#include <iostream>

using namespace std;

// تعريف مسار قاعدة البيانات الثابت (يمكنك تغييره إلى المسار الفعلي)
const string DBConnection::DB_PATH = "clinic_data.db";

// -----------------------------------------------------------------
// دوال ثابتة (Static Functions)
// -----------------------------------------------------------------

sqlite3* DBConnection::openDB() {
    sqlite3 *db = nullptr;
    int rc = sqlite3_open(DB_PATH.c_str(), &db);

    if (rc != SQLITE_OK) {
        cerr << "DB Error: Cannot open database: " << sqlite3_errmsg(db) << endl;
        if (db) sqlite3_close(db);
        return nullptr;
    }

    // تفعيل Foreign Keys لضمان سلامة العلاقات
    char *errMsg = 0;
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", nullptr, nullptr, &errMsg);
    if (errMsg) {
        sqlite3_free(errMsg);
    }
    
    return db;
}

void DBConnection::closeDB(sqlite3* db) {
    if (db) {
        sqlite3_close(db);
    }
}

// دالة تنفيذ (INSERT, UPDATE, DELETE)
bool DBConnection::execute(sqlite3* db, const string& sql) {
    if (!db) return false;
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "SQL Execute Error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

// دالة استعلام (SELECT)
bool DBConnection::query(sqlite3* db, const string& sql, 
                          int (*callback)(void*, int, char**, char**), 
                          void* data) {
    if (!db) return false;
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql.c_str(), callback, data, &errMsg);

    if (rc != SQLITE_OK) {
        cerr << "SQL Query Error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}