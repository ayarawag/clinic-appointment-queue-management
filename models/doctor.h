#ifndef DOCTOR_H
#define DOCTOR_H
#include <string>
#include <vector>

class Doctor {
public:
    int id;
    std::string name, specialization, schedule;

    Doctor();
    Doctor(std::string n, std::string s);

    bool create(const std::string& db="clinic.db");
    bool update(const std::string& db="clinic.db");
    bool remove(const std::string& db="clinic.db");
    bool setSchedule(std::string s, const std::string& db="clinic.db");

    static Doctor loadById(int id,const std::string& db="clinic.db");
    static std::vector<Doctor> getAll(const std::string& db="clinic.db");
};

#endif