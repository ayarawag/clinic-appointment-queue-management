#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>

class Doctor {
public:
    int id;
    std::string name;
    std::string specialty;

    Doctor();
    Doctor(int id, std::string name, std::string specialty);

    bool registerDoctor(const std::string& db="clinic.db");
    bool update(const std::string& db="clinic.db");
    bool remove(const std::string& db="clinic.db");

    static Doctor loadById(int id, const std::string& db="clinic.db");
};

#endif