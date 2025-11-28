#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <sqlite3.h>
#include <string>
using namespace std;

class DBConnection {
public:
    sqlite3* db;
    DBConnection(const string& filename);
    ~DBConnection();

    bool execute(const string& sql);
    bool query(const string& sql, int (*callback)(void*, int, char**, char**), void* data);
};

#endif