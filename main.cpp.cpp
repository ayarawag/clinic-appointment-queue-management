#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <limits>
#include <sqlite3.h>

using namespace std;

// ----------------- Utilities -----------------
string hashPassword(const string& password) {
    string hashed = "";
    for (char c : password) hashed += to_string((int)c + 7);
    return hashed;
}

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
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
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
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
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

    void showQueue(int doctorID) {
        const char* sql = "SELECT appointmentID, patientID, date, time, emergency, status, queue_position "
                          "FROM appointments WHERE doctorID=? AND status!='Cancelled' ORDER BY queue_position ASC;";
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, doctorID);
        cout << "\n--- Queue for Doctor " << doctorID << " ---\n";
        bool any = false;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            any = true;
            cout << "Pos " << sqlite3_column_int(stmt, 6)
                 << ": ApptID=" << sqlite3_column_int(stmt, 0)
                 << " PatientID=" << sqlite3_column_int(stmt, 1)
                 << " Date=" << sqlite3_column_text(stmt, 2)
                 << " Time=" << sqlite3_column_text(stmt, 3)
                 << " Emergency=" << (sqlite3_column_int(stmt, 4) ? "Yes" : "No")
                 << " Status=" << sqlite3_column_text(stmt, 5) << endl;
        }
        if (!any) cout << "No appointments.\n";
        sqlite3_finalize(stmt);
    }

    int nextQueuePosition(int doctorID) {
        const char* sql = "SELECT COALESCE(MAX(queue_position),0)+1 FROM appointments WHERE doctorID=? AND status!='Cancelled';";
        sqlite3_stmt* stmt = nullptr;
        int pos = 1;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, doctorID);
        if (sqlite3_step(stmt) == SQLITE_ROW) pos = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return pos;
    }

    bool isSlotAvailable(int doctorID, const string& date, const string& timeStr) {
        const char* sql = "SELECT COUNT(*) FROM appointments WHERE doctorID=? AND date=? AND time=? AND status!='Cancelled';";
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, doctorID);
        sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, timeStr.c_str(), -1, SQLITE_TRANSIENT);
        bool available = true;
        if (sqlite3_step(stmt) == SQLITE_ROW) available = (sqlite3_column_int(stmt, 0) == 0);
        sqlite3_finalize(stmt);
        return available;
    }
    void reorderQueue(int doctorID) {
        const char* sql = "SELECT appointmentID FROM appointments WHERE doctorID=? AND status!='Cancelled' ORDER BY emergency DESC, bookingTime ASC;";
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, doctorID);
        vector<int> ordered;
        while (sqlite3_step(stmt) == SQLITE_ROW)
            ordered.push_back(sqlite3_column_int(stmt, 0));
        sqlite3_finalize(stmt);

        const char* upd = "UPDATE appointments SET queue_position=? WHERE appointmentID=?;";
        sqlite3_stmt* ustmt = nullptr;
        sqlite3_prepare_v2(db, upd, -1, &ustmt, nullptr);
        for (size_t i = 0; i < ordered.size(); ++i) {
            sqlite3_reset(ustmt);
            sqlite3_bind_int(ustmt, 1, i + 1);
            sqlite3_bind_int(ustmt, 2, ordered[i]);
            sqlite3_step(ustmt);
        }
        sqlite3_finalize(ustmt);
    }

    int bookAppointment(int patientID) {
        int doctorID; string date, timeStr, em;
        cout << "\n--- Book Appointment ---\n";
        cout << "Enter Doctor ID: "; cin >> doctorID; cin.ignore();
        cout << "Enter Date (YYYY-MM-DD): "; getline(cin, date);
        cout << "Enter Time (HH:MM AM/PM): "; getline(cin, timeStr);
        cout << "Emergency? (y/n): "; getline(cin, em);
        bool emergency = (em == "y" || em == "Y");

        if (!emergency && !isSlotAvailable(doctorID, date, timeStr)) {
            cout << "Slot already booked.\n"; return -1;
        }

        int queuePos = nextQueuePosition(doctorID);
        const char* sql = "INSERT INTO appointments(patientID,doctorID,date,time,status,bookingTime,emergency,queue_position)"
                          "VALUES(?,?,?,?,?,?,?,?);";
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, patientID);
        sqlite3_bind_int(stmt, 2, doctorID);
        sqlite3_bind_text(stmt, 3, date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, timeStr.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, "Booked", -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 6, nowEpoch());
        sqlite3_bind_int(stmt, 7, emergency ? 1 : 0);
        sqlite3_bind_int(stmt, 8, queuePos);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        reorderQueue(doctorID);
        cout << "Appointment booked successfully!\n";
        return 0;
    }

    bool cancelAppointment(int apptID) {
        const char* sql = "UPDATE appointments SET status='Cancelled' WHERE appointmentID=?;";
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, apptID);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        cout << "Appointment " << apptID << " cancelled.\n";
        return true;
    }

    void manualReorderQueue(int doctorID) {
        showQueue(doctorID);
        cout << "Enter appointment IDs in new order separated by spaces (end with 0): ";
        vector<int> ids;
        int id;
        while (cin >> id && id != 0) ids.push_back(id);
        cin.ignore();
        const char* upd = "UPDATE appointments SET queue_position=? WHERE appointmentID=?;";
        sqlite3_stmt* stmt = nullptr;
        sqlite3_prepare_v2(db, upd, -1, &stmt, nullptr);
        for (size_t i = 0; i < ids.size(); ++i) {
            sqlite3_reset(stmt);
            sqlite3_bind_int(stmt, 1, i + 1);
            sqlite3_bind_int(stmt, 2, ids[i]);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
        cout << "Queue reordered successfully.\n";
    }

    void doctorViewTools(int doctorID) {
        while (true) {
            cout << "\n--- Doctor Menu ---\n1. Show queue\n2. Cancel appointment\n3. Back\nChoice: ";
            string in; getline(cin, in);
            if (in.empty()) continue;
            int c = stoi(in);
            if (c == 1) showQueue(doctorID);
            else if (c == 2) {cout << "Enter appointment ID: "; string sid; getline(cin, sid);
                if (!sid.empty()) cancelAppointment(stoi(sid));
            }
            else if (c == 3) break;
        }
    }
};

// ----------------- MAIN -----------------
int main() {
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
        else if (ch == 2) {
            auto info = user.login();
            if (info.first == -1) continue;
            string role = info.second;

            if (role == "patient") {
                while (true) {
                    cout << "\n--- Patient Menu ---\n1. Book Appointment\n2. Cancel Appointment\n3. Back\nChoice: ";
                    string p; getline(cin, p);
                    if (p.empty()) continue;
                    int c = stoi(p);
                    if (c == 1) app.bookAppointment(info.first);
                    else if (c == 2) {
                        cout << "Enter appointment ID: "; string sid; getline(cin, sid);
                        if (!sid.empty()) app.cancelAppointment(stoi(sid));
                    }
                    else if (c == 3) break;
                }
            } else if (role == "doctor") {
                app.doctorViewTools(info.first);
            } else if (role == "admin") { 
                while (true) {
                    cout << "\n--- Admin Menu ---\n1. View Queue\n2. Reorder Queue\n3. Back\nChoice: ";
                    string a; getline(cin, a);
                    if (a.empty()) continue;
                    int c = stoi(a);
                    if (c == 1) {
                        cout << "Doctor ID: "; string sid; getline(cin, sid);
                        if (!sid.empty()) app.showQueue(stoi(sid));
                    }
                    else if (c == 2) {
                        cout << "Doctor ID: "; string sid; getline(cin, sid);
                        if (!sid.empty()) app.manualReorderQueue(stoi(sid));
                    }
                    else if (c == 3) break;
                }
            }
        } else if (ch == 3) break;
    }

    sqlite3_close(db);
    return 0;
}