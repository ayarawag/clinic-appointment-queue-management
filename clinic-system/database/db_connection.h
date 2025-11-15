#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <sqlite3.h>
#include <string>
using namespace std;

class DB {
public:
    static sqlite3* connect(string filename);
};

#endif