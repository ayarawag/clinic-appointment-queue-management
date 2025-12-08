#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <utility>

class Queue {
private:
    std::vector<std::pair<int,int>> queueList; // appointment_id, position
public:
    void addPatient(int appointmentId);
    void reorder(int appointmentId, int newPos);
    void refreshPositionsDB(const std::string& db="clinic.db");
    std::vector<std::pair<int,int>> getCurrentQueue() const;
};

#endif