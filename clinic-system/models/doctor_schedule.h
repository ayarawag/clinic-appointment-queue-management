#ifndef DOCTOR_SCHEDULE_H
#define DOCTOR_SCHEDULE_H

#include <string>
using namespace std;

class DoctorSchedule {
public:
    int id;
    int doctorID;
    string day;
    string startTime;
    string endTime;

    DoctorSchedule() = default;

    DoctorSchedule(int id, int doctorID, string day, string startTime, string endTime);
};

#endif