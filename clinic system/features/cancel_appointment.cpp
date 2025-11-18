#include "../models/appointment.h"
#include <iostream>

void cancelAppointment() {
    int appointmentId;
    std::cout << "Appointment ID to cancel: "; std::cin >> appointmentId;
    Appointment app;
    app.id = appointmentId;
    if(app.cancel()) std::cout << "Appointment canceled!\n";
}