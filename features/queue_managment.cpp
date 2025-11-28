#include "../models/queue.h"
#include <iostream>
using namespace std;

static Queue globalQueue;

void updateQueue() {
    int pid;
    cout << "Patient ID to add: "; cin >> pid;
    globalQueue.addPatient(pid);
    globalQueue.refreshPositionsDB();
    cout << "Added to queue.\n";
}

void reorderQueue() {
    int pid, pos;
    cout << "Patient ID to move: "; cin >> pid;
    cout << "New position: "; cin >> pos;
    globalQueue.reorder(pid, pos);
    globalQueue.refreshPositionsDB();
    cout << "Queue reordered.\n";
}