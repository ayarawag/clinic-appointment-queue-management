#include "../models/doctor.h"
#include <iostream>
using namespace std;

void setDoctorSchedule() {
    int id; cout << "Doctor ID: "; cin >> id;
    cin.ignore();
    string s; cout << "Schedule (e.g. Mon-Fri 09:00-12:00): "; getline(cin, s);
    Doctor d; d.id = id; d.setSchedule(s);
    cout << "Schedule updated.\n";
}