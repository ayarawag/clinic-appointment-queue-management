#include "notification.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <iostream>
#include <sstream>
#include <stdexcept> // لإضافة معالجة الاستثناءات

Notification::Notification() : patientId(0), message("") {}

Notification::Notification(int aid, std::string msg) {
    patientId = aid;
    message = msg;
}

// ==========================================================
// 1. الدالة sendNotification: تطبيق Singleton و Try/Catch
// ==========================================================
bool Notification::sendNotification(const std::string& db) {
    // التحقق الأساسي من المدخلات
    if (patientId <= 0 || message.empty()) {
        std::cerr << "ERROR (Validation): Cannot send notification with invalid patient ID or empty message.\n";
        return false;
    }
    
    try {
        // [Singleton] استخدام getInstance للحصول على نسخة الاتصال الوحيدة
        DBConnection* conn = DBConnection::getInstance(db);

        std::ostringstream q;
        q << "INSERT INTO notifications(patientId, message) VALUES("
          << patientId << ",'" << message << "');";

        // [Singleton] استخدام المؤشر -> لتنفيذ الاستعلام
        return conn->execute(q.str());
        
    } catch (const std::exception& e) {
        // [Try/Catch] معالجة أي استثناء (مثل فشل الاتصال)
        std::cerr << "EXCEPTION CAUGHT (SendNotification): DB operation failed: " << e.what() << std::endl;
        return false;
    }
}

// ==========================================================
// 2. الدالة runReminders: تطبيق Singleton و Try/Catch
// ==========================================================
void Notification::runReminders(const std::string& db) {
    try {
        // [Singleton] استخدام getInstance للحصول على نسخة الاتصال الوحيدة
        DBConnection* conn = DBConnection::getInstance(db);

        std::cout << "Starting reminder process...\n";

        // Placeholder: just run a simple SELECT
        // [Singleton] استخدام المؤشر -> لتنفيذ الاستعلام
        conn->query(
            "SELECT id FROM notifications;",
            [](void*, int, char**, char**) -> int { 
                // يمكنك هنا إضافة منطق لمعالجة كل إشعار
                return 0; 
            },
            nullptr
        );

        std::cout << "Reminders processed successfully.\n";
        
    } catch (const std::exception& e) {
        // [Try/Catch] معالجة أي استثناء
        std::cerr << "CRITICAL EXCEPTION CAUGHT (RunReminders): DB process failed: " << e.what() << std::endl;
        // يمكننا اختيار إرجاع قيمة هنا إذا كانت الدالة ليست void
    }
}