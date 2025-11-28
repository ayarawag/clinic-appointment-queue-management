#include "db_connection.h"
#include <iostream>
using namespace std;

DBConnection::DBConnection(const string& filename) {
    if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        cerr << "DB Error: " << sqlite3_errmsg(db) << "\n";
        db = nullptr;
    }
}

DBConnection::~DBConnection() {
    if (db) sqlite3_close(db);
}

bool DBConnection::execute(const string& sql) {
    if (!db) return false;
    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "SQL Error: " << (errMsg ? errMsg : "") << "\n";
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DBConnection::query(const string& sql, int (*callback)(void*, int, char**, char**), void* data) {
    if (!db) return false;
    char* errMsg = nullptr;
    return sqlite3_exec(db, sql.c_str(), callback, data, &errMsg) == SQLITE_OK;
}