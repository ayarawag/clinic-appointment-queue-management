#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <utility>
#include <string>

class Queue {
public:
    std::vector<std::pair<int,int>> queueList; 
    // pair = (appointment_id , position)

    void addPatient(int appointmentId);
    void reorder(int appointmentId, int newPos);
    
    // هذه الدالة ستستخدم Singleton و Try/Catch
    void refreshPositionsDB(const std::string& db = "clinic.db"); 
    
    std::vector<std::pair<int,int>> getCurrentQueue() const;

private:
    bool exists(int appointmentId) const;
    void normalizePositions();
};

#endif