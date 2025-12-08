#include "../database/db_connection.h"
#include "../utils/password_utils.h"
#include <iostream>
#include <string>
#include <limits>     // ← هذا هو المهم
#include <vector>

bool loginUser() {
    std::string email, password;
    std::cout << "Enter your email: ";
    std::cin >> email;
    std::cout << "Enter your password: ";
    std::cin >> password;

    DBConnection db("clinic.db");
    std::string storedHash;
    std::string q = "SELECT password_hash FROM patients WHERE email='" + email + "' LIMIT 1;";
    db.query(q, [](void* ud, int cols, char** vals, char**) -> int {
        std::string* s = (std::string*)ud;
        if (vals[0]) *s = vals[0];
        return 0;
    }, &storedHash);

    if (!storedHash.empty() && verifyPassword(password, storedHash)) {
        std::cout << "Login successful.\n";
        return true;
    } else {
        std::cout << "Invalid credentials.\n";
        return false;
    }
}