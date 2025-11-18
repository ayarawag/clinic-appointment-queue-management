#include "doctor_schedule.h"

DoctorSchedule::DoctorSchedule(int id, int doctorID, string day, string startTime, string endTime)
    : id(id), doctorID(doctorID), day(day), startTime(startTime), endTime(endTime) {}