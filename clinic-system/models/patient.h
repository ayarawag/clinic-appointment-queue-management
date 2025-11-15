#ifndef PATIENT_H
#define PATIENT_H

#include <string>
using namespace std;

class Patient {
public:
    int id;
    string name;
    string email;
    string phone;
    string password;

    Patient() {}
    Patient(int id, string name, string email, string phone, string password);

    void displayInfo();
};

#endif