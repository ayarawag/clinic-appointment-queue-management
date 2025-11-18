#include "queue.h"
#include <algorithm>

void Queue::addPatient(int patientId) {
    patientIds.push_back(patientId);
}

void Queue::removePatient(int patientId) {
    patientIds.erase(std::remove(patientIds.begin(), patientIds.end(), patientId), patientIds.end());
}

int Queue::getPosition(int patientId) {
    for(size_t i=0; i<patientIds.size(); i++)
        if(patientIds[i]==patientId) return i+1;
    return -1;
}