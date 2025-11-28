#include "queue.h"
#include "../database/db_connection.h"
#include <algorithm>
#include <iostream>
using namespace std;

void Queue::addPatient(int patientId) {
    patientIds.push_back(patientId);
}

void Queue::removePatient(int patientId) {
    patientIds.erase(remove(patientIds.begin(), patientIds.end(), patientId), patientIds.end());
}

int Queue::getPosition(int patientId) {
    for (size_t i = 0; i < patientIds.size(); ++i)
        if (patientIds[i] == patientId) return i + 1;
    return -1;
}

void Queue::reorder(int patientId, int newPos) {
    removePatient(patientId);
    if (newPos <= 0) newPos = 1;
    if ((size_t)newPos > patientIds.size()) patientIds.push_back(patientId);
    else patientIds.insert(patientIds.begin() + (newPos - 1), patientId);
}

void Queue::refreshPositionsDB() {
    DBConnection db("clinic.db");
    db.execute("DELETE FROM queue;");
    for (size_t i = 0; i < patientIds.size(); ++i) {
        int pid = patientIds[i];
        string sql = "INSERT INTO queue(patientId,position) VALUES(" + to_string(pid) + "," + to_string((int)i+1) + ");";
        db.execute(sql);
    }
}