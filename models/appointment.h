#ifndef APPOINTMENT_H
#define APPOINTMENT_H
#include <string>
#include <vector>

class Appointment {
public:
    int id, patientId, doctorId, paid;
    std::string dateTime, status;

    Appointment();

    bool book(const std::string& db="clinic.db");
    bool cancel(const std::string& db="clinic.db");
    bool reschedule(std::string newDate,const std::string& db="clinic.db");
    bool setPaid(const std::string& db="clinic.db");

    static Appointment loadById(int id,const std::string& db="clinic.db");
    static std::vector<Appointment> getByPatient(int pid,const std::string& db="clinic.db");
    static std::vector<Appointment> getByDoctor(int did,const std::string& db="clinic.db");
};

#endif