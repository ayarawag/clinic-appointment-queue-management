#include "../models/doctor.h"
#include <iostream>

void setDoctorSchedule() {
    int doctorId;
    std::string schedule;
    std::cout << "Doctor ID: "; std::cin >> doctorId;
    std::cout << "Schedule: "; std::cin >> schedule;

    Doctor doc;
    doc.id = doctorId;
    doc.setSchedule(schedule);
    std::cout << "Doctor schedule updated!\n";
}