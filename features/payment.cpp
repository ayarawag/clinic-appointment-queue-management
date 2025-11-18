#include "../models/appointment.h"
#include <iostream>

void payAppointment() {
    int appointmentId;
    std::cout << "Appointment ID to pay: "; std::cin >> appointmentId;
    Appointment app;
    app.id = appointmentId;
    app.paid = true;
    std::cout << "Payment successful!\n";
}