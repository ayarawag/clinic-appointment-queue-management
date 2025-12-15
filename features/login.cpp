#include "../database/db_connection.h"
#include "../utils/password_utils.h"
#include <iostream>
#include <string>

bool loginUser() {
    std::string email, password;

    std::cout << "Enter your email: ";
    std::cin >> email;

    std::cout << "Enter your password: ";
    std::cin >> password;

    DBConnection db("clinic.db");

    std::string storedHash = "";

    // الاستعلام الصحيح
    std::string q = 
        "SELECT password_hash FROM patients WHERE email='" + email + "' LIMIT 1;";

    // قراءة نتيجة الاستعلام
    db.query(q,
        [](void* out, int cols, char** vals, char**) -> int {
            if (vals[0])
                *((std::string*)out) = vals[0];
            return 0;
        },
        &storedHash
    );

    if (storedHash.empty()) {
        std::cout << "Email not found.\n";
        return false;
    }

    // مقارنة كلمة السر
    if (PasswordUtils::verifyPassword(password, storedHash)) {
        std::cout << "Login successful.\n";
        return true;
    } else {
        std::cout << "Invalid password.\n";
        return false;
    }
}