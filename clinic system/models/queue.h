#ifndef QUEUE_H
#define QUEUE_H
#include <vector>

class Queue {
public:
    std::vector<int> patientIds;

    void addPatient(int patientId);
    void removePatient(int patientId);
    int getPosition(int patientId);
};

#endif