#include "db_connection.h"
#include <iostream>

DBConnection::DBConnection(const std::string& filename) {
    if (sqlite3_open(filename.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Failed to open DB: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

DBConnection::~DBConnection() {
    if (db) sqlite3_close(db);
}

bool DBConnection::execute(const std::string& query) {
    char* errMsg = nullptr;

    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);

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

    if (rc != SQLITE_OK) {
        std::cerr << "DB Query Error: " << (errMsg ? errMsg : "Unknown") << std::endl;
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}