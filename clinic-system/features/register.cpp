#include <iostream>
#include "utils.h"       // استدعاء دوال التحقق
#include "../database/db_connection.h"

void registerPatient(sqlite3* db) {
    std::string name, email, phone, password;

    std::cout << "Enter name: ";
    std::cin >> name;

    std::cout << "Enter email: ";
    std::cin >> email;
    if (!isValidEmail(email)) {
        std::cout << "❌ Invalid email format!\n";
        return;
    }

    std::cout << "Enter phone: ";
    std::cin >> phone;
    if (!isValidPhone(phone)) {
        std::cout << "❌ Invalid phone format!\n";
        return;
    }

    std::cout << "Enter password: ";
    std::cin >> password;
    if (!isStrongPassword(password)) {
        std::cout << "❌ Password too weak!\n";
        return;
    }

    // هنا يمكنك إضافة الكود لإدخال البيانات في قاعدة البيانات
    std::cout << "✅ Registration successful!\n";
}
