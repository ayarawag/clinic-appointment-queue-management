#ifndef PATIENT_H
#define PATIENT_H

#include <string>

class Patient {
public:
    int id;
    std::string name;
    std::string phone;
    std::string email;
    std::string passwordHash;

    Patient();
    Patient(std::string n, std::string p, std::string e, std::string pass);

    bool registerPatient(const std::string& db = "clinic.db");
    bool update(const std::string& db = "clinic.db");
    bool remove(const std::string& db = "clinic.db");

    static bool emailExists(std::string email, const std::string& dbfile = "clinic.db");
    static Patient loadById(int id, const std::string& db = "clinic.db");
};

#endif