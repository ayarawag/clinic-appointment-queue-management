#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
using namespace std;

class Appointment {
public:
    int id;
    int patientId;
    int doctorId;
    string date;
    string time;
    string status;

    Appointment() {}
    Appointment(int id, int patientId, int doctorId, string date, string time, string status);
};

#endif