#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>

class Appointment {
public:
    int id;
    int patientId;
    int doctorId;
    std::string dateTime;
    bool paid = false;

    bool book();
    bool cancel();
    bool reschedule(const std::string& newTime);
};

#endif