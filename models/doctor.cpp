#include "doctor.h"
#include "../database/db_connection.h"
#include <iostream>
#include <sstream>

using namespace std;

// ----------------------------------------------------
// تطبيق دوال Doctor
// ----------------------------------------------------

// ... (باقي تطبيقات المنشئات و Getters/Setters هنا) ...
// إذا كان لديك Doctor::Doctor() و Doctor::getId() الخ، احتفظي بها.

// دالة تحديث جدول الطبيب
void Doctor::setSchedule(const std::string& newSchedule) {
    this->schedule = newSchedule;
    
    // 1. فتح الاتصال (Static Call)
    sqlite3 *db = DBConnection::openDB();
    if (!db) return;

    // 2. بناء استعلام SQL
    std::string sql = "UPDATE doctors SET schedule = '" + newSchedule + "' WHERE id = " + std::to_string(this->id) + ";";

    // 3. تنفيذ الاستعلام (Static Call) وإغلاق الاتصال
    bool success = DBConnection::execute(db, sql);
    DBConnection::closeDB(db);

    if (success) {
        cout << "Doctor " << this->name << " schedule updated successfully." << endl;
    } else {
        cerr << "Failed to update doctor schedule." << endl;
    }
}

// دالة جلب بيانات الطبيب بواسطة المعرف
bool Doctor::getDoctorById(int doctorId) {
    // 1. تعريف الهيكل المساعد وفتح الاتصال
    struct DoctorData {
        Doctor* d;
        bool found = false;
    };
    DoctorData data = {this, false};

    sqlite3 *db = DBConnection::openDB();
    if (!db) return false;

    // 2. بناء استعلام SQL
    std::string sql = "SELECT name, specialization, schedule FROM doctors WHERE id = " + std::to_string(doctorId) + ";";

    // دالة Callback لمعالجة النتائج
    auto callback = [](void* dataPtr, int cols, char** vals, char** azColName) -> int {
        DoctorData* d = (DoctorData*)dataPtr;
        if (cols >= 3) {
            d->found = true;
            // يجب تعيين id هنا أيضاً لو كان متاحاً
            // d->d->id = doctorId; // (سنقوم بتعيينه في الخارج)

            if (vals[0]) d->d->name = vals[0];
            if (vals[1]) d->d->specialization = vals[1];
            if (vals[2]) d->d->schedule = vals[2];
        }
        return 0;
    };

    // 3. تنفيذ الاستعلام (Static Call) وإغلاق الاتصال
    DBConnection::query(db, sql, callback, &data);
    DBConnection::closeDB(db);

    // إذا تم العثور على الطبيب، قم بتعيين المعرف (ID)
    if (data.found) {
        this->id = doctorId;
        return true;
    }
    return false;
}