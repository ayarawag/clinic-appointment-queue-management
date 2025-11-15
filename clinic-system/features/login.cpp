#include <iostream>
#include <sqlite3.h>
#include "../database/db_connection.h"
using namespace std;

bool validEmailLogin(const string& email) {
    return email.find("@") != string::npos;
}

bool validPasswordLogin(const string& password) {
    return password.length() >= 8;
}

void loginPatient(sqlite3* db) {
    string email, password;

    cout << "\n=== Login ===\n";

    cout << "Email: ";
    cin >> email;
    if (!validEmailLogin(email)) {
        cout << "❌ Invalid email!\n";
        return;
    }

    cout << "Password: ";
    cin >> password;
    if (!validPasswordLogin(password)) {
        cout << "❌ Password must be at least 8 characters.\n";
        return;
    }

    string sql =
        "SELECT id, name FROM patients WHERE email='" + email +
        "' AND password='" + password + "'";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        cout << "✅ Login successful!\n";
        cout << "Welcome, " << sqlite3_column_text(stmt, 1) << "!\n";
    }
    else {
        cout << "❌ Wrong email or password!\n";
    }

    sqlite3_finalize(stmt);
}