#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
using namespace std;

class Queue {
public:
    vector<int> patientIds;
    void addPatient(int patientId);
    void removePatient(int patientId);
    int getPosition(int patientId);
    void reorder(int patientId, int newPos);
    void refreshPositionsDB();
};

#endif