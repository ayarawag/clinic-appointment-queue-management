#ifndef DOCTOR_H
#define DOCTOR_H
#include <string>

class Doctor {
public:
    int id;
    std::string name;
    std::string specialization;

    void setSchedule(const std::string& schedule);
};

#endif