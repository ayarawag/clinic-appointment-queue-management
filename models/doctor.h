#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
using namespace std;

class Doctor {
public:
    int id;
    string schedule;
    void setSchedule(const string& schedule);
};

#endif