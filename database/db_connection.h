#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include "sqlite3.h"
#include <string>
#include <iostream>

using namespace std;

class DBConnection {
private:
    // المسار الافتراضي لقاعدة البيانات
    static const string DB_PATH; 

public:
    // دالة ثابتة لفتح الاتصال، تُرجع مؤشر sqlite3*
    static sqlite3* openDB();

    // دالة ثابتة لإغلاق الاتصال
    static void closeDB(sqlite3* db);

    // دالة تنفيذ (EXECUTE) ثابتة: لتنفيذ INSERT, UPDATE, DELETE
    static bool execute(sqlite3* db, const string& sql);

    // دالة استعلام (QUERY) ثابتة: لتنفيذ SELECT
    static bool query(sqlite3* db, const string& sql, 
                      int (*callback)(void*, int, char**, char**), 
                      void* data);
};

#endif // DB_CONNECTION_H