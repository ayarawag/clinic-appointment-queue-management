#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>

class Appointment {
public:
    int id;
    int patientId;
    int doctorId;
    std::string dateTime;
    std::string status; // Scheduled / Cancelled / Rescheduled

    Appointment();

    bool book(const std::string& db = "clinic.db");
    bool cancel(const std::string& db = "clinic.db");
    bool reschedule(const std::string& newTime, const std::string& db = "clinic.db");

    static Appointment loadById(int id, const std::string& db = "clinic.db");
};

#endif