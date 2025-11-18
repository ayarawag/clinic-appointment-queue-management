#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <limits>
#include <sqlite3.h>
#include <algorithm>

using namespace std;

// ----------------- Utilities -----------------
string hashPassword(const string& password) {
    string hashed = "";
    for (char c : password) hashed += to_string((int)c + 7);
    return hashed;
}

// دالة تصحيحية للتحقق من البريد الإلكتروني
bool isValidEmail(const string& email) {
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.', atPos);
    return (atPos != string::npos && dotPos != string::npos && atPos > 0 && dotPos < email.length() - 1);
}

bool isValidPassword(const string& password) {
    return password.length() >= 8;
}

long long nowEpoch() { return static_cast<long long>(time(nullptr)); }

// ----------------- UserManager -----------------
class UserManager {
    sqlite3* db;
public:
    UserManager(sqlite3* _db) : db(_db) {}

    bool initTables() {
        const char* sqlUsers = R"(
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                phone TEXT UNIQUE,
                email TEXT UNIQUE,
                password TEXT NOT NULL,
                role TEXT NOT NULL
            );
        )";
        char* err = nullptr;
        if (sqlite3_exec(db, sqlUsers, nullptr, nullptr, &err) != SQLITE_OK) {
            cerr << "Error creating users table: " << (err ? err : "") << endl;
            sqlite3_free(err);
            return false;
        }
        return true;
    }

    int registerUser() {
        string name, phone, email, password, role;
        cout << "\n--- Register ---\n";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Enter your name: "; getline(cin, name);
        cout << "Enter your phone: "; getline(cin, phone);
        cout << "Enter your email: "; getline(cin, email);
        cout << "Choose role (admin / doctor / patient): "; getline(cin, role);
        for (auto &c : role) c = tolower(c);
        if (role != "admin" && role != "doctor" && role != "patient") {
            cout << "Invalid role.\n"; return -1;
        }
        cout << "Enter password (min 8 chars): "; getline(cin, password);

        if (!email.empty() && !isValidEmail(email)) { cout << "Invalid email format.\n"; return -1; }
        if (!isValidPassword(password)) { cout << "Password must be at least 8 characters.\n"; return -1; }

        const char* sql = "INSERT INTO users (name, phone, email, password, role) VALUES (?, ?, ?, ?, ?);";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cout << "DB error (prepare): " << sqlite3_errmsg(db) << endl;
            return -1;
        }
        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, phone.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_TRANSIENT);
        string hashed = hashPassword(password);
        sqlite3_bind_text(stmt, 4, hashed.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, role.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cout << "Error inserting user: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return -1;
        }
        sqlite3_finalize(stmt);
        int newID = static_cast<int>(sqlite3_last_insert_rowid(db));
        cout << "Registration successful! Your ID: " << newID << " (" << role << ")\n";
        return newID;
    }

    pair<int,string> login() {
        string emailOrPhone, password;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "\n--- Login ---\n";
        cout << "Enter email or phone: "; getline(cin, emailOrPhone);
        cout << "Enter password: "; getline(cin, password);

        const char* sql = "SELECT id, name, password, role FROM users WHERE email=? OR phone=?;";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cout << "DB error (prepare): " << sqlite3_errmsg(db) << endl;
            return {-1, ""};
        }
        sqlite3_bind_text(stmt, 1, emailOrPhone.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, emailOrPhone.c_str(), -1, SQLITE_TRANSIENT);

        int userID = -1; string role;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string dbPass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            if (dbPass == hashPassword(password)) {
                userID = sqlite3_column_int(stmt, 0);
                role = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
                cout << "Welcome " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) << " (" << role << ")\n";
            } else cout << "Wrong password.\n";
        } else cout << "User not found.\n";
        sqlite3_finalize(stmt);
        return {userID, role};
    }
};

// ----------------- AppointmentManager -----------------
class AppointmentManager {
    sqlite3* db;
public:
    AppointmentManager(sqlite3* _db) : db(_db) {}

    bool initTables() {
        const char* sqlAppointments = R"(
            CREATE TABLE IF NOT EXISTS appointments (
                appointmentID INTEGER PRIMARY KEY AUTOINCREMENT,
                patientID INTEGER NOT NULL,
                doctorID INTEGER NOT NULL,
                date TEXT NOT NULL,
                time TEXT NOT NULL,
                status TEXT NOT NULL,
                bookingTime INTEGER NOT NULL,
                emergency INTEGER DEFAULT 0,
                queue_position INTEGER,
                FOREIGN KEY(patientID) REFERENCES users(id),
                FOREIGN KEY(doctorID) REFERENCES users(id)
            );
        )";
        char* err = nullptr;
        if (sqlite3_exec(db, sqlAppointments, nullptr, nullptr, &err) != SQLITE_OK) {
            cerr << "Error creating appointments table: " << (err ? err : "") << endl;
            sqlite3_free(err);
            return false;
        }
        return true;
    }
    // باقي دوال AppointmentManager كما هي من الكود الأصلي
};

// ----------------- MAIN -----------------
int main() {
    cout << ">>> Clinic System <<<" << endl;
    sqlite3* db = nullptr;
    if (sqlite3_open("clinic.db", &db) != SQLITE_OK) {
        cerr << "Failed to open DB\n"; return 1;
    }

    UserManager user(db);
    AppointmentManager app(db);
    user.initTables();
    app.initTables();

    while (true) {
        cout << "\n=== Clinic System ===\n";
        cout << "1. Register\n2. Login\n3. Exit\nChoice: ";
        string input;
        getline(cin, input);
        if (input.empty()) continue;
        int ch = stoi(input);

        if (ch == 1) user.registerUser();
        else if (ch == 2) user.login();
        else if (ch == 3) break;
    }

    sqlite3_close(db);
    return 0;
}
