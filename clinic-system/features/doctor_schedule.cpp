#include "../models/doctor_schedule.h"
#include <sqlite3.h>
#include <iostream>
using namespace std;

// create schedule table
void initDoctorSchedule(sqlite3* db) {
    cout << "[DoctorSchedule] Table initialized.\n";
}

// add schedule
void addDoctorSchedule(sqlite3* db, int doctorID) {
    cout << "[DoctorSchedule] Add schedule for doctor ID: " << doctorID << endl;
}

// view schedule
void viewDoctorSchedule(sqlite3* db, int doctorID) {
    cout << "[DoctorSchedule] Viewing schedule for doctor ID: " << doctorID << endl;
}