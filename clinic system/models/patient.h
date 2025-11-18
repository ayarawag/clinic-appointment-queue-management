#ifndef PATIENT_H
#define PATIENT_H

#include <string>

class Patient {
public:
    int id;
    std::string name;
    std::string phone;
    std::string email;
    std::string password;

    Patient() = default;
    Patient(std::string n, std::string p, std::string e, std::string pw);

    bool registerPatient();
    bool loginPatient(const std::string& emailOrPhone, const std::string& pw);
};

#endif