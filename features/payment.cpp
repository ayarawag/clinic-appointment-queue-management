#include "../models/appointment.h"
#include "../database/db_connection.h"
#include <iostream>
#include <chrono>
using namespace std;

bool simulatePayment(const string& details) {
    if (details.empty()) return false;
    char last = details.back();
    if (!isdigit(last)) return false;
    int d = last - '0';
    return (d % 2 == 0); // even -> success, odd -> fail
}

void payAppointment() {
    int id;
    cout << "Appointment ID: "; cin >> id;
    cout << "Payment method (card/paypal/gpay/apple): ";
    string method; cin >> method;
    cout << "Payment details (card number or account): ";
    string details; cin >> details;

    bool ok = simulatePayment(details);
    DBConnection db("clinic.db");
    long long now = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();

    if (ok) {
        Appointment a; a.id = id;
        if (a.setPaid()) {
            cout << "Payment successful.\n";
            string rec = "INSERT INTO receipts(appointmentId,method,status,message,createdAt) VALUES(" + to_string(id) + ",'" + method + "','success','Paid', " + to_string(now) + ");";
            db.execute(rec);
            cout << "Receipt recorded.\n";
        }
    } else {
        cout << "Payment failed: declined or invalid details.\n";
        string rec = "INSERT INTO receipts(appointmentId,method,status,message,createdAt) VALUES(" + to_string(id) + ",'" + method + "','failed','Declined', " + to_string(now) + ");";
        db.execute(rec);
    }
}