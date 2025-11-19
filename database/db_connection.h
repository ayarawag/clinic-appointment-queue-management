#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <string>
#include <iostream>

class DBConnection {
public:
    DBConnection(const std::string& dbFile) {
        std::cout << "Pretend opening DB: " << dbFile << "\n";
    }

    bool isOpen() const { return true; }
    bool execute(const std::string& query) {
        std::cout << "Pretend executing query: " << query << "\n";
        return true;
    }
};

#endif