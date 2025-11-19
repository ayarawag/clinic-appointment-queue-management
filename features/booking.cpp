#include "../models/appointment.h"
#include <iostream>

void bookAppointment() {
    int patientId, doctorId;
    std::string dateTime;
    std::cout << "Patient ID: "; std::cin >> patientId;
    std::cout << "Doctor ID: "; std::cin >> doctorId;
    std::cout << "Date & Time: "; std::cin >> dateTime;

    Appointment app;
    app.patientId = patientId;
    app.doctorId = doctorId;
    app.dateTime = dateTime;
    if(app.book()) std::cout << "Appointment booked!\n";
}