#include <iostream>
using namespace std;
#include "../models/appointment.h"

void test_appointment_booking() {
    std::cout << "[TEST] Appointment Booking...\n";

    Appointment a;
    a.patientId = 1;
    a.doctorId = 1;
    a.dateTime = "2025-01-01 10:00:00";

    bool ok = a.book("clinic.db");

    if (ok)
        std::cout << "PASS: Appointment booked.\n";
    else
        std::cout << "FAIL: Booking failed.\n";
}

void test_appointment_load() {
    std::cout << "[TEST] Load Appointment...\n";

    Appointment a = Appointment::loadById(1);

    if (a.id == 1)
        std::cout << "PASS: Appointment loaded.\n";
    else
        std::cout << "FAIL: Load failed.\n";
}

void test_appointment_cancel() {
    std::cout << "[TEST] Cancel Appointment...\n";

    Appointment a = Appointment::loadById(1);
    bool ok = a.cancel("clinic.db");

    if (ok)
        std::cout << "PASS: Cancelled.\n";
    else
        std::cout << "FAIL: Cancel failed.\n";
}

void test_appointment_reschedule() {
    std::cout << "[TEST] Appointment Reschedule...\n";

    Appointment a = Appointment::loadById(1);
    bool ok = a.reschedule("2025-01-02 14:00:00", "clinic.db");

    if (ok)
        std::cout << "PASS: Rescheduled.\n";
    else
        std::cout << "FAIL: Reschedule failed.\n";
}