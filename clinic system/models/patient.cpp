#include "patient.h"
#include "../database/db_connection.h"
#include <iostream>

Patient::Patient(std::string n, std::string p, std::string e, std::string pw) {
    name = n; phone = p; email = e; password = pw;
}

bool Patient::registerPatient() {
    DBConnection db("clinic.db");
    std::string query = "INSERT INTO patients(name, phone, email, password) VALUES('" + 
                         name + "','" + phone + "','" + email + "','" + password + "');";
    return db.execute(query);
}

bool Patient::loginPatient(const std::string& emailOrPhone, const std::string& pw) {
    DBConnection db("clinic.db");
    // التحقق من البيانات - يمكن تطويره لاحقاً
    return true;
}