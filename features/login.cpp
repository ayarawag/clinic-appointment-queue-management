#include "../models/patient.h"
#include <iostream>
#include <string>
using namespace std;

/**
 * @brief تقوم بتسجيل دخول المستخدم.
 * @return true في حالة نجاح تسجيل الدخول، false في حالة الفشل.
 */
bool loginUser() {
    string email, password;
    
    cout << "=========================\n";
    cout << "      تسجيل الدخول\n";
    cout << "=========================\n";

    cout << "البريد الإلكتروني: ";
    cin >> email;

    cout << "كلمة المرور: ";
    cin >> password;

    // إنشاء كائن Patient فارغ لاستخدام دالة العضو
    Patient p;

    // استدعاء دالة login للتحقق من البيانات في قاعدة البيانات
    if (p.login(email, password)) { 
        cout << "تم تسجيل الدخول بنجاح!\n";
        return true;
    } else {
        cout << "بريد إلكتروني أو كلمة مرور غير صحيحة.\n";
        return false;
    }
}