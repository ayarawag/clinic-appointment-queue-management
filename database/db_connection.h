#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <string>
#include "sqlite3.h"

class DBConnection {
private:
    sqlite3* db;
public:
    DBConnection(const std::string& filename);
    ~DBConnection();

    bool execute(const std::string& query);
    bool query(const std::string& query,
               int (*callback)(void*, int, char**, char**),
               void* data);
};

#endif