#include "patient.h"
#include "../database/db_connection.h" 

#include <string> 
#include <iostream> 
#include <sstream> 

using namespace std; 

// ----------------------------------------------------
// تطبيق المنشئات و Getters/Setters
// ----------------------------------------------------

Patient::Patient() : id(0), name(""), phone(""), email(""), password("") {}

Patient::Patient(int id, const std::string& name, const std::string& phone, const std::string& email, const std::string& password)
    : id(id), name(name), phone(phone), email(email), password(password) {}

int Patient::getId() const { return id; }
std::string Patient::getName() const { return name; }
std::string Patient::getPhone() const { return phone; }
std::string Patient::getEmail() const { return email; }
std::string Patient::getPassword() const { return password; }

void Patient::setName(const std::string& name) { this->name = name; }
void Patient::setPhone(const std::string& phone) { this->phone = phone; }
void Patient::setEmail(const std::string& email) { this->email = email; }
void Patient::setPassword(const std::string& password) { this->password = password; }


// ----------------------------------------------------
// تطبيق دوال قاعدة البيانات (Database Functions)
// ----------------------------------------------------

/**
 * @brief دالة تسجيل مريض جديد.
 */
bool Patient::registerPatient(const std::string& name, const std::string& phone, const std::string& email, const std::string& password) {
    // 1. فتح الاتصال
    sqlite3 *db = DBConnection::openDB(); 
    if (!db) {
        return false; // فشل فتح القاعدة تم الإبلاغ عنه داخل openDB
    }

    // 2. بناء استعلام SQL
    std::string sql = "INSERT INTO patients (name, phone, email, password) VALUES ('" + name + "', '" + phone + "', '" + email + "', '" + password + "');";

    // 3. تنفيذ الاستعلام وإغلاق الاتصال
    bool success = DBConnection::execute(db, sql);
    DBConnection::closeDB(db); 

    if (success) {
        cout << "Patient " << name << " registered successfully." << endl;
    }
    return success;
}

/**
 * @brief دالة تسجيل دخول المريض.
 */
bool Patient::login(const std::string& email, const std::string& password) {
    // 1. فتح الاتصال
    sqlite3 *db = DBConnection::openDB();
    if (!db) {
        return false;
    }

    // 2. بناء استعلام SQL
    std::string sql = "SELECT id FROM patients WHERE email = '" + email + "' AND password = '" + password + "';";
    
    int rowCount = 0;
    
    // دالة Callback لمعالجة نتائج استعلام SELECT
    auto callback = [](void* countPtr, int argc, char** argv, char** azColName) -> int {
        int* count = static_cast<int*>(countPtr);
        if (argc > 0) {
            // يمكننا الحصول على الـ ID هنا لو احتجنا، لكن نكتفي بالعد
            (*count)++;
        }
        return 0; 
    };

    // 3. تنفيذ الاستعلام وإغلاق الاتصال
    DBConnection::query(db, sql, callback, &rowCount);
    DBConnection::closeDB(db);

    // إذا كان rowCount > 0، فذلك يعني أن بيانات الدخول صحيحة
    return rowCount > 0;
}