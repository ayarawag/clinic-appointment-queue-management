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
    string preferredChannel; // email / sms / app

    long long nowEpoch();
    bool isValidEmail(const string& email);
    string hashPassword(const string& input);

public:
    Patient(string n, string p, string e, string pw);

    bool registerPatient();
    bool loginPatient(const string& user, const string& pw);
};

#endif