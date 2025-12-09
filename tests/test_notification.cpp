#include <iostream>
using namespace std;

#include "../models/notification.h"
#include "../database/db_connection.h"

void test_notification_create() {
    cout << "[TEST] Notification create...\n";

    Notification n(5, "Your appointment is tomorrow.");

    if (n.appointmentId == 5 && n.message == "Your appointment is tomorrow.")
        cout << "PASS: Constructor OK.\n";
    else
        cout << "FAIL: Constructor FAILED.\n";
}

void test_notification_send() {
    cout << "[TEST] Notification send...\n";

    Notification n(10, "Reminder test");
    bool ok = n.sendNotification("clinic.db");

    if (ok)
        cout << "PASS: sendNotification OK.\n";
    else
        cout << "FAIL: sendNotification FAILED.\n";
}

void test_notification_runReminders() {
    cout << "[TEST] Notification reminders...\n";

    bool ok = true;
    try {
        Notification::runReminders("clinic.db");
    } catch (...) {
        ok = false;
    }

    if (ok)
        cout << "PASS: runReminders OK.\n";
    else
        cout << "FAIL: runReminders FAILED.\n";
}