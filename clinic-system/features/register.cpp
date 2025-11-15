#include <iostream>
#include <regex>
#include <sqlite3.h>
#include "../database/db_connection.h"
using namespace std;

// =============================
// Helper: Email Validation
// =============================
bool isValidEmail(const string& email) {
    const regex pattern("(\\w+)(\\.\\w+)*@(\\w+)(\\.\\w+)+");
    return regex_match(email, pattern);
}

// =============================
// Helper: Phone Validation
// =============================
bool isValidPhone(const string& phone) {
    for (char c : phone)
        if (!isdigit(c)) return false;

    return phone.length() >= 8 && phone.length() <= 15;
}

// =============================
// Helper: Password Validation
// =============================
bool isStrongPassword(const string& password) {
    return password.length() >= 8;
}

// =============================
// Check if user exists
// =============================
bool userExists(sqlite3* db, const string& email) {
    string sql = "SELECT id FROM patients WHERE email='" + email + "'";
    sqlite3_stmt* stmt;

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);

    sqlite3_finalize(stmt);
    return exists;
}

// =============================
// Registration Function
// =============================
void registerPatient(sqlite3* db) {
    string name, email, phone, password;

    cout << "\n=== Registration ===\n";

    cout << "Enter name: ";
    cin >> name;

    cout << "Enter email: ";
    cin >> email;
    if (!isValidEmail(email)) {
        cout << "❌ Invalid email format!\n";
        return;
    }

    cout << "Enter phone: ";
    cin >> phone;
    if (!isValidPhone(phone)) {
        cout << "❌ Phone must be numbers only (8–15 digits)\n";
        return;
    }

    cout << "Enter password: ";
    cin >> password;
    if (!isStrongPassword(password)) {
        cout << "❌ Password must be at least 8 characters!\n";
        return;
    }

    if (userExists(db, email)) {
        cout << "❌ This email already exists!\n";
        return;
    }

    string sql =
        "INSERT INTO patients(name,email,phone,password) VALUES('" +
        name + "','" + email + "','" + phone + "','" + password + "')";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), NULL, NULL, &errMsg);

    if (rc == SQLITE_OK)
        cout << "✅ Registration successful!\n";
    else
        cout << "❌ Error: " << errMsg << endl;
}