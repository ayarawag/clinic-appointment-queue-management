#include "doctor.h"
#include "../database/db_connection.h"
#include <iostream>
using namespace std;

void Doctor::setSchedule(const string& schedule) {
    DBConnection db("clinic.db");
    string sql = "UPDATE doctors SET schedule='" + schedule + "' WHERE id=" + to_string(id) + ";";
    if (!db.execute(sql)) cout << "Failed to update schedule.\n";
}