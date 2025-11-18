#include "doctor.h"
#include "../database/db_connection.h"

void Doctor::setSchedule(const std::string& schedule) {
    DBConnection db("clinic.db");
    std::string query = "UPDATE doctors SET schedule='" + schedule + "' WHERE id=" + std::to_string(id) + ";";
    db.execute(query);
}