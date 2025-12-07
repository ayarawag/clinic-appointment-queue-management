#include "../models/patient.h"
#include <iostream>
#include <string> 
using namespace std;

/**
 * @brief تقوم بتسجيل مستخدم جديد (مريض).
 */
void registerUser() {
    string name, phone, email, password;
    
    cout << "==========================\n";
    cout << "    تسجيل مستخدم جديد\n";
    cout << "==========================\n";
    
    cout << "الاسم بالكامل: "; 
    cin.ignore(10000, '\n'); 
    getline(cin, name);
    
    cout << "رقم الهاتف: ";
    cin >> phone;
    
    cout << "البريد الإلكتروني: ";
    cin >> email;
    
    cout << "كلمة المرور: ";
    cin >> password;

    // 1. إنشاء كائن Patient فارغ
    Patient p; 

    // 2. استدعاء الدالة registerPatient مع تمرير 4 وسائط
    if (p.registerPatient(name, phone, email, password)) {
        cout << "تم التسجيل بنجاح.\n";
    } else {
        cout << "فشل التسجيل. ربما البريد الإلكتروني مستخدم بالفعل أو حدث خطأ في قاعدة البيانات.\n";
    }
}