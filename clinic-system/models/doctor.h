#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
using namespace std;

class Doctor {
public:
    int id;
    string name;
    string specialization;

    Doctor() {}
    Doctor(int id, string name, string specialization);
};

#endif