#include "queue.h"
#include "../database/db_connection.h"
#include <sstream>
#include <iostream>

void Queue::addPatient(int appointmentId) {
    int pos = queueList.size() + 1;
    queueList.push_back({appointmentId, pos});
}

void Queue::reorder(int appointmentId, int newPos) {
    for (auto &p : queueList) {
        if (p.first == appointmentId) {
            p.second = newPos;
            break;
        }
    }
}

void Queue::refreshPositionsDB(const std::string& dbfile) {
    DBConnection db(dbfile);

    for (auto &p : queueList) {
        std::ostringstream q;
        q << "UPDATE appointments SET queue_position=" << p.second
          << " WHERE id=" << p.first;

        db.execute(q.str());
    }
}

std::vector<std::pair<int,int>> Queue::getCurrentQueue() const {
    return queueList;
}