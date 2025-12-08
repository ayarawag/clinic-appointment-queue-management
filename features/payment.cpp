#include "../models/appointment.h"
#include "../database/db_connection.h"
#include <iostream>
#include <string>
#include <chrono>

bool simulatePayment(const std::string& details) {
    if (details.empty()) return false;
    char last = details.back();
    if (!isdigit(last)) return false;
    int d = last - '0';
    return (d % 2 == 0);
}

void payAppointmentCLI() {
    int id; std::cout << "Appointment ID: "; std::cin >> id;
    std::cout << "Payment method (card/paypal/gpay/apple): ";
    std::string method; std::cin >> method;
    std::cout << "Payment details (card number or account): ";
    std::string details; std::cin >> details;

    bool ok = simulatePayment(details);
    DBConnection db("clinic.db");
    long long now = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();

    if (ok) {
        Appointment a = Appointment::loadById(id);
        if (a.id == 0) { std::cout << "Appointment not found.\n"; return; }
        a.id = id;
        if (a.setPaid()) {
            std::cout << "Payment successful.\n";
            std::string rec = "INSERT INTO receipts(appointmentId,method,status,message,createdAt) VALUES(" +
                              std::to_string(id) + ",'" + method + "','success','Paid'," + std::to_string(now) + ");";
            db.execute(rec);
            std::cout << "Receipt recorded.\n";
        } else {
            std::cout << "Could not mark appointment as paid (DB error).\n";
        }
    } else {
        std::cout << "Payment failed: declined or invalid details.\n";
        std::string rec = "INSERT INTO receipts(appointmentId,method,status,message,createdAt) VALUES(" +
                          std::to_string(id) + ",'" + method + "','failed','Declined'," + std::to_string(now) + ");";
        db.execute(rec);
    }
}