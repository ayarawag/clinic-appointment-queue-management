#include "notification.h"
#include "../database/db_connection.h" 
#include <iostream>
#include <sstream>
#include <stdexcept> 

Notification::Notification() : patientId(0), message("") {}

Notification::Notification(int aid, std::string msg) {
    patientId = aid;
    message = msg;
}

// ==========================================================
// 1. الدالة sendNotification: تطبيق Singleton و Try/Catch
// ==========================================================
bool Notification::sendNotification(const std::string& db) {
    if (patientId <= 0 || message.empty()) {
        std::cerr << "ERROR (Validation): Cannot send notification with invalid patient ID or empty message.\n";
        return false;
    }
    
    try {
        DBConnection* conn = DBConnection::getInstance(db);

        std::ostringstream q;
        q << "INSERT INTO notifications(patientId, message) VALUES("
          << patientId << ",'" << message << "');";

        return conn->execute(q.str());
        
    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (SendNotification): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 2. الدالة runReminders: تطبيق Singleton و Try/Catch
// ==========================================================
void Notification::runReminders(const std::string& db) {
    try {
        DBConnection* conn = DBConnection::getInstance(db);

        std::cout << "Starting reminder process...\n";

        conn->query(
            "SELECT id FROM notifications;",
            [](void*, int, char**, char**) -> int { 
                return 0; 
            },
            nullptr
        );

        std::cout << "Reminders processed successfully.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "CRITICAL EXCEPTION CAUGHT (RunReminders): DB process failed: " << e.what() << std::endl;
    }
}