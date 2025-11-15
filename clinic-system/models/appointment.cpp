#include "appointment.h"

Appointment::Appointment(int id, int patientId, int doctorId, string date, string time, string status)
    : id(id), patientId(patientId), doctorId(doctorId), date(date), time(time), status(status) {}