#include "../models/appointment.h"
#include <iostream>
using namespace std;

void cancelAppointment() {
    int id;
    cout << "Appointment ID: "; cin >> id;
    Appointment a; a.id = id;
    if (a.cancel()) cout << "Appointment canceled.\n";
    else cout << "Cancel failed.\n";
}