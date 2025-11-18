#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H
#include <sqlite3.h>
#include <string>

class DBConnection {
public:
    sqlite3* db;

    DBConnection(const std::string& dbFile);
    ~DBConnection();
    bool execute(const std::string& query);
};

#endif