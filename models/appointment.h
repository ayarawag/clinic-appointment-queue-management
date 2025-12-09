#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>

class Appointment {
public:
    int id;
    int patientId;
    int doctorId;
    std::string dateTime;
    int paid;

    Appointment();
    Appointment(int pid, int did, std::string datetime);

    bool book(const std::string& db = "clinic.db");
    bool cancel(const std::string& db = "clinic.db");
    bool reschedule(const std::string& newDateTime, const std::string& db = "clinic.db");
    bool setPaid(bool paidValue = true, const std::string& db = "clinic.db");

    static Appointment loadById(int id, const std::string& db = "clinic.db");
};

#endif