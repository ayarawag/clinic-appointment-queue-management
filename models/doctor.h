#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
#include <vector>   //  جديد: لإرجاع قائمة الأوقات المتاحة

class Doctor {
public:
    int id;
    std::string name;
    std::string specialization;
    std::string schedule;

    Doctor();
    Doctor(int id, std::string name, std::string spec);
    Doctor(std::string name, std::string spec);

    // الدوال الأساسية (بدون أي تغيير)
    bool registerDoctor(const std::string& db = "clinic.db");
    bool update(const std::string& db = "clinic.db");
    bool remove(const std::string& db = "clinic.db");
    bool setSchedule(const std::string& sched, const std::string& db = "clinic.db");

    static Doctor loadById(int id, const std::string& db = "clinic.db");

    // Feature: View Available Slots for a doctor on a given date
    static std::vector<std::string> getAvailableSlots(
        int doctorId,
        const std::string& date,
        const std::string& db = "clinic.db"
    );
};

#endif