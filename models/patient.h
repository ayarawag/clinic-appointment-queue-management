#ifndef PATIENT_H
#define PATIENT_H

#include <string>
using namespace std;

class Patient {
private:
    string name;
    string phone;
    string email;
    string password;

    int failedAttempts;
    long long lockedUntil;
    string preferredChannel;

public:
    // -------------------------------
    // 1) Default constructor (مهم للاختبارات)
    // -------------------------------
    Patient();

    // -------------------------------
    // 2) Main constructor
    // -------------------------------
    Patient(string n, string p, string e, string pw);

    // -------------------------------
    // 3) Validation + Register
    // -------------------------------
    bool isValidEmail(const string& email);
    bool registerPatient();

    // -------------------------------
    // 4) Login
    // -------------------------------
    bool loginPatient(const string& user, const string& pw);

    // -------------------------------
    // 5) Password hashing (بسيط)
    // -------------------------------
    string hashPassword(const string& input);

    // -------------------------------
    // 6) Get current time (seconds)
    // -------------------------------
    static long long nowEpoch();
};

#endif