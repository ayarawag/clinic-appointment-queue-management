#include "db_connection.h"
#include <iostream>

DBConnection::DBConnection(const std::string& dbFile) {
    if(sqlite3_open(dbFile.c_str(), &db)) {
        std::cerr << "Can't open DB: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

DBConnection::~DBConnection() {
    if(db) sqlite3_close(db);
}

bool DBConnection::execute(const std::string& query) {
    char* errMsg = nullptr;
    if(sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL Error: " << errMsg << std::endl;
        return false;
    }
    return true;
}