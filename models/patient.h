#ifndef PATIENT_H
#define PATIENT_H

#include <string>

class Patient {
public:
    std::string name, phone, email, password;

    Patient() = default;
    Patient(std::string n, std::string p, std::string e, std::string pw);

    bool registerPatient();
    bool loginPatient(const std::string& emailOrPhone, const std::string& pw);
};

#endif