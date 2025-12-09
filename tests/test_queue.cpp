#include <iostream>
using namespace std;
#include "../models/queue.h"

void test_queue_add() {
    std::cout << "[TEST] Queue Add...\n";

    Queue q;
    q.addPatient(10);
    q.addPatient(20);
    q.addPatient(30);

    auto list = q.getCurrentQueue();

    if (list.size() == 3 &&
        list[0].first == 10 &&
        list[1].first == 20 &&
        list[2].first == 30)
        std::cout << "PASS: Queue add OK.\n";
    else
        std::cout << "FAIL: Queue add FAILED.\n";
}

void test_queue_reorder() {
    std::cout << "[TEST] Queue Reorder...\n";

    Queue q;
    q.addPatient(10); // pos 1
    q.addPatient(20); // pos 2
    q.addPatient(30); // pos 3

    q.reorder(20, 1);

    auto list = q.getCurrentQueue();

    // find patient 20 → check if pos changed to 1
    bool ok = false;
    for (auto &p : list)
        if (p.first == 20 && p.second == 1)
            ok = true;

    if (ok)
        std::cout << "PASS: Queue reorder OK.\n";
    else
        std::cout << "FAIL: Queue reorder FAILED.\n";
}

void test_queue_refresh_db() {
    std::cout << "[TEST] Queue Refresh DB...\n";

    Queue q;
    q.addPatient(1);
    q.addPatient(2);

    bool ok = true;
    try {
        q.refreshPositionsDB("clinic.db");
    } catch (...) {
        ok = false;
    }

    if (ok)
        std::cout << "PASS: Refresh DB OK.\n";
    else
        std::cout << "FAIL: Refresh DB FAILED.\n";
}