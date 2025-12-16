#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>

class Doctor {
public:
    int id;
    std::string name;
    std::string specialization;
    std::string schedule;

    Doctor();
    Doctor(int id, std::string name, std::string spec);

    // NEW:
    Doctor(std::string name, std::string spec);

    bool registerDoctor(const std::string& db = "clinic.db");
    bool update(const std::string& db = "clinic.db");
    bool remove(const std::string& db = "clinic.db");
    bool setSchedule(const std::string& sched, const std::string& db = "clinic.db"); // NEW

    static Doctor loadById(int id, const std::string& db = "clinic.db");
};

#endif