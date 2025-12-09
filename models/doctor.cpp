#include "doctor.h"
#include "../database/db_connection.h"
#include <sstream>
#include <string>
#include <cstdlib>

Doctor::Doctor() {
    id = 0;
}

Doctor::Doctor(int id_, std::string name_, std::string specialty_) {
    id = id_;
    name = name_;
    specialty = specialty_;
}

bool Doctor::registerDoctor(const std::string& db) {
    DBConnection x(db);
    std::ostringstream q;

    q << "INSERT INTO doctors(name, specialty) VALUES('"
      << name << "','" << specialty << "');";

    return x.execute(q.str());
}

Doctor Doctor::loadById(int docId, const std::string& db) {
    Doctor d;
    DBConnection x(db);

    std::string query =
        "SELECT id, name, specialty FROM doctors WHERE id=" + std::to_string(docId);

    x.query(
        query,
        [](void* out, int cols, char** vals, char**) -> int {
            Doctor* d = (Doctor*)out;

            if (vals[0]) d->id = std::stoi(vals[0]);
            if (vals[1]) d->name = vals[1];
            if (vals[2]) d->specialty = vals[2];

            return 0;
        },
        &d
    );

    return d;
}

bool Doctor::update(const std::string& db) {
    if (id == 0) return false;

    DBConnection x(db);
    std::ostringstream q;

    q << "UPDATE doctors SET name='" << name
      << "', specialty='" << specialty
      << "' WHERE id=" << id;

    return x.execute(q.str());
}

bool Doctor::remove(const std::string& db) {
    if (id == 0) return false;

    DBConnection x(db);
    return x.execute("DELETE FROM doctors WHERE id=" + std::to_string(id));
}