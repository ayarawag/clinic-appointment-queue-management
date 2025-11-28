#include "../models/appointment.h"
#include <iostream>
using namespace std;

void bookAppointment() {
    int pid, did;
    string date;
    cout << "Patient ID: "; cin >> pid;
    cout << "Doctor ID: "; cin >> did;
    cin.ignore();
    cout << "Date & Time (YYYY-MM-DD HH:MM): "; getline(cin, date);

    Appointment a;
    a.patientId = pid;
    a.doctorId = did;
    a.dateTime = date;
    if (a.book()) cout << "Appointment booked.\n";
    else cout << "Booking failed.\n";
}