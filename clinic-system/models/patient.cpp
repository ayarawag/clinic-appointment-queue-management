#include "patient.h"
#include <iostream>
using namespace std;

Patient::Patient(int id, string name, string email, string phone, string password)
    : id(id), name(name), email(email), phone(phone), password(password) {}

void Patient::displayInfo() {
    cout << "Patient: " << name << " | Email: " << email << " | Phone: " << phone << endl;
}