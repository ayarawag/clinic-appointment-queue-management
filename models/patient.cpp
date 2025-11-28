#include "patient.h"
#include "../database/db_connection.h"
#include <iostream>
#include <regex>
#include <chrono>
using namespace std;

Patient::Patient(string n, string p, string e, string pw) {
    name = n;
    phone = p;
    email = e;
    password = pw;
    failedAttempts = 0;
    lockedUntil = 0;
    preferredChannel = "app";
}

long long Patient::nowEpoch() {
    return chrono::duration_cast<chrono::seconds>(
        chrono::system_clock::now().time_since_epoch()
    ).count();
}

bool Patient::isValidEmail(const string& email) {
    const regex pattern(R"((^[\w.%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$))");
    return regex_match(email, pattern);
}

// simple student-style hash (+3)
string Patient::hashPassword(const string& input) {
    string hashed = "";
    for (char c : input) hashed += char(c + 3);
    return hashed;
}

bool Patient::registerPatient() {
    if (!isValidEmail(email)) {
        cout << "Invalid email format.\n";
        return false;
    }
    if (password.length() < 8) {
        cout << "Password must be at least 8 characters.\n";
        return false;
    }

    DBConnection db("clinic.db");
    string hp = hashPassword(password);

    string sql = "INSERT INTO patients(name,phone,email,password,failedAttempts,lockedUntil,preferredChannel) "
                 "VALUES('" + name + "','" + phone + "','" + email + "','" + hp + "',0,0,'app');";

    return db.execute(sql);
}

bool Patient::loginPatient(const string& user, const string& pw) {

    DBConnection db("clinic.db");

    // ================================
    // 1) Fetch patient by email/phone
    // ================================
    struct Row {
        int id = 0;
        string storedPw = "";
        int failed = 0;
        long long locked = 0;
    } row;

    string sql = "SELECT id, password, failedAttempts, lockedUntil "
                 "FROM patients WHERE email='" + user + "' OR phone='" + user + "';";

    db.query(sql,
        [](void* data, int cols, char** vals, char**) -> int {
            Row* r = (Row*)data;
            if (vals[0]) r->id = stoi(vals[0]);
            if (vals[1]) r->storedPw = vals[1];
            if (vals[2]) r->failed = stoi(vals[2]);
            if (vals[3]) r->locked = stoll(vals[3]);
            return 0;
        }, &row
    );

    if (row.id == 0) {
        cout << "User not found.\n";
        return false;
    }

    // ================================
    // 2) Check if locked
    // ================================
    long long now = nowEpoch();
    if (row.locked > now) {
        cout << "Account is locked. Try again later.\n";
        return false;
    }

    // ================================
    // 3) Compare passwords
    // ================================
    string hpw = hashPassword(pw);

    if (hpw == row.storedPw) {
        // reset failed attempts
        db.execute("UPDATE patients SET failedAttempts=0, lockedUntil=0 WHERE id=" + to_string(row.id) + ";");
        cout << "Login successful.\n";
        return true;
    }

    // ================================
    // 4) Wrong password
    // ================================
    row.failed++;

    if (row.failed >= 5) {
        long long lockTime = now + (15 * 60); // 15 min
        string sqlUpdate = "UPDATE patients SET failedAttempts=" + to_string(row.failed) +
                           ", lockedUntil=" + to_string(lockTime) +
                           " WHERE id=" + to_string(row.id) + ";";
        db.execute(sqlUpdate);
        cout << "Too many attempts. Account locked for 15 minutes.\n";
    } 
    else {
        string sqlUpdate = "UPDATE patients SET failedAttempts=" +
                           to_string(row.failed) +
                           " WHERE id=" + to_string(row.id) + ";";
        db.execute(sqlUpdate);

        cout << "Invalid password. Attempts = " << row.failed << "/5\n";
    }

    return false;
}