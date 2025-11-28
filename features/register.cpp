#include "../models/patient.h"
#include <iostream>
using namespace std;

void registerUser() {
    string name, phone, email, password;
    cout << "Name: "; cin.ignore(); getline(cin, name);
    cout << "Phone: "; cin >> phone;
    cout << "Email: "; cin >> email;
    cout << "Password: "; cin >> password;

    Patient p(name, phone, email, password);
    if (p.registerPatient()) cout << "Registered successfully.\n";
    else cout << "Registration failed.\n";
}