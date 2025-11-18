#include "appointment.h"
#include "../database/db_connection.h"
#include <iostream>

bool Appointment::book() {
    DBConnection db("clinic.db");
    std::string query = "INSERT INTO appointments(patientId, doctorId, dateTime, paid) VALUES(" +
                         std::to_string(patientId) + "," + std::to_string(doctorId) + ",'" + dateTime + "',0);";
    return db.execute(query);
}

bool Appointment::cancel() {
    DBConnection db("clinic.db");
    std::string query = "DELETE FROM appointments WHERE id=" + std::to_string(id) + ";";
    return db.execute(query);
}

bool Appointment::reschedule(const std::string& newTime) {
    DBConnection db("clinic.db");
    std::string query = "UPDATE appointments SET dateTime='" + newTime + "' WHERE id=" + std::to_string(id) + ";";
    return db.execute(query);
}