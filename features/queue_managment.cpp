#include "../models/queue.h"
#include <iostream>

void updateQueue() {
    int patientId;
    std::cout << "Patient ID to add to queue: "; std::cin >> patientId;
    Queue q;
    q.addPatient(patientId);
    std::cout << "Queue updated!\n";
}