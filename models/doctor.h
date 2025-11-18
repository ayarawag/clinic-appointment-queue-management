#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>

class Doctor {
public:
    int id;
    void setSchedule(const std::string& schedule);
};

#endif