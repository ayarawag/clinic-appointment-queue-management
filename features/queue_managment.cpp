#include "../models/queue.h"
#include <iostream>
#include <string>
#include <limits>     // ← هذا هو المهم
#include <vector>
static Queue globalQueue;

void addToQueueCLI() {
    int appointmentId;
    std::cout << "Appointment ID to add to queue: "; std::cin >> appointmentId;
    if (appointmentId <= 0) { std::cout << "Invalid appointment id.\n"; return; }
    globalQueue.addPatient(appointmentId);
    globalQueue.refreshPositionsDB();
    std::cout << "Added to queue.\n";
}

void reorderQueueCLI() {
    int appointmentId, newPos;
    std::cout << "Appointment ID to move: "; std::cin >> appointmentId;
    std::cout << "New position: "; std::cin >> newPos;
    if (appointmentId <= 0 || newPos <= 0) { std::cout << "Invalid input.\n"; return; }
    globalQueue.reorder(appointmentId, newPos);
    globalQueue.refreshPositionsDB();
    std::cout << "Queue reordered.\n";
}

void showQueueCLI() {
    auto q = globalQueue.getCurrentQueue();
    if (q.empty()) { std::cout << "Queue is empty.\n"; return; }
    std::cout << "Current Queue:\n";
    for (auto &p : q) {
        std::cout << "Appointment ID: " << p.first << " | Position: " << p.second << "\n";
    }
}