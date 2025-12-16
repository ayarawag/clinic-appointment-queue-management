#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <string>
#include "sqlite3.h"

class DBConnection {
private:
    sqlite3* db;
    // [تعديل Singleton 1] إضافة مؤشر ثابت يحمل النسخة الوحيدة
    static DBConnection* instance; 
    
    // [تعديل Singleton 2] جعل المُنشئ خاصاً (private) لمنع الإنشاء الخارجي
    DBConnection(const std::string& filename); 
    
    // [تعديل Singleton 3] جعل الهادم خاصاً (private) للتحكم في الحذف
    ~DBConnection(); 

public:
    // [تعديل Singleton 4] دالة ثابتة عامة للوصول إلى النسخة الوحيدة
    static DBConnection* getInstance(const std::string& filename);

    // [تعديل Singleton 5] منع النسخ والتعيين (لضمان بقاء النسخة وحيدة)
    DBConnection(const DBConnection&) = delete;
    DBConnection& operator=(const DBConnection&) = delete;

    // الدوال العامة تبقى كما هي، لكنها الآن تُستدعى عبر المؤشر
    bool execute(const std::string& query);
    bool query(const std::string& query,
               int (*callback)(void*, int, char**, char**),
               void* data);
    
    // [إضافة اختيارية] دالة لحذف النسخة عند نهاية البرنامج (للتنظيف)
    static void destroyInstance();
};

#endif