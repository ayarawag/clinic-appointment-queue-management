#include "doctor.h"
#include "../database/db_connection.h"
#include <sstream>

Doctor::Doctor() : id(0), name(""), specialty(""), schedule("") {}

Doctor::Doctor(int i, std::string n, std::string s) {
    id = i;
    name = n;
    specialty = s;
    schedule = "";
}

// NEW CONSTRUCTOR
Doctor::Doctor(std::string n, std::string s) {
    id = 0;
    name = n;
    specialty = s;
    schedule = "";
}

bool Doctor::registerDoctor(const std::string& db) {
    DBConnection conn(db);
    std::ostringstream q;

    q << "INSERT INTO doctors(name, specialty) VALUES('"
      << name << "','" << specialty << "');";

    return conn.execute(q.str());
}

Doctor Doctor::loadById(int did, const std::string& db) {
    Doctor d;
    DBConnection conn(db);

    std::string q =
        "SELECT id,name,specialty,schedule FROM doctors WHERE id=" + std::to_string(did);

    conn.query(
        q,
        [](void* out, int, char** vals, char**) -> int {
            Doctor* d = (Doctor*)out;

            if (vals[0]) d->id = std::stoi(vals[0]);
            if (vals[1]) d->name = vals[1];
            if (vals[2]) d->specialty = vals[2];
            if (vals[3]) d->schedule = vals[3];

            return 0;
        },
        &d);

    return d;
}

bool Doctor::setSchedule(const std::string& sched, const std::string& db) {
    if (id == 0) return false;

    DBConnection conn(db);
    std::ostringstream q;

    q << "UPDATE doctors SET schedule='" << sched << "' WHERE id=" << id;

    return conn.execute(q.str());
}