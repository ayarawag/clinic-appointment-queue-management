#include "db_connection.h"
#include <iostream>

sqlite3* DB::connect(string filename) {
    sqlite3* db;
    int rc = sqlite3_open(filename.c_str(), &db);

    if (rc != SQLITE_OK) {
        cout << "Database Error: " << sqlite3_errmsg(db) << endl;
        return nullptr;
    }

    return db;
}