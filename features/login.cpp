#include "../models/patient.h"
#include <iostream>
#include <string>
using namespace std;

bool loginUser() {
    string email, password;

    // طلب بيانات المستخدم
    cout << "Enter your email: ";
    cin >> email;

    cout << "Enter your password: ";
    cin >> password;

    // إنشاء object للـ Patient بالبيانات المدخلة
    Patient p("", "", email, password);  


    bool loginSuccess = false;  // افتراضياً فشل
    // مثال للتوضيح:
    // loginSuccess = checkCredentialsInDB(p);

    if(loginSuccess) {
        cout << "Login successful!\n";
        return true;
    } else {
        cout << "Invalid email or password.\n";
        return false;
    }
}