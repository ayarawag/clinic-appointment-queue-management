#include "../models/appointment.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <iostream>
#include <string>
#include <chrono>
#include <sstream> 
#include <stdexcept> // لمعالجة الاستثناءات

// دالة محاكاة الدفع تبقى كما هي
bool simulatePayment(const std::string& details) {
    if (details.empty()) return false;

    char last = details.back();
    if (!isdigit(last)) return false;

    int d = last - '0';
    return (d % 2 == 0); // أرقام زوجية = قبول
}

// ==========================================================
// الدالة payAppointmentCLI: تطبيق Singleton و Try/Catch
// ==========================================================
void payAppointmentCLI() {
    int id;
    std::cout << "Appointment ID: ";
    if (!(std::cin >> id)) {
        std::cerr << "Invalid ID input.\n";
        return;
    }

    std::cout << "Payment method (card/paypal/gpay/apple): ";
    std::string method;
    std::cin >> method;

    std::cout << "Payment details (card number or account): ";
    std::string details;
    std::cin >> details;

    bool payment_successful = simulatePayment(details);
    long long now = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    
    // [Singleton] الحصول على نسخة الاتصال الوحيدة
    const std::string DB_FILE = "clinic.db";
    DBConnection* db = DBConnection::getInstance(DB_FILE); 
    
    // ==========================================================
    // محيط Try/Catch لجميع عمليات DB اللاحقة
    // ==========================================================
    try {
        if (payment_successful) {
            // 1. تحميل الموعد
            // (Appointment::loadById تستخدم Singleton الآن و Try/Catch)
            Appointment a = Appointment::loadById(id, DB_FILE); 

            if (a.id == 0) {
                std::cout << "Payment failed: Appointment not found (ID: " << id << ").\n";
                return; 
            }

            // 2. تحديث حالة الدفع في جدول Appointments
            // (a.setPaid تستخدم Singleton و Try/Catch الآن)
            if (a.setPaid(true, DB_FILE)) {
                std::cout << "Payment successful.\n";

                // 3. تسجيل الإيصال (النجاح)
                std::string rec =
                    "INSERT INTO receipts(appointmentId, method, status, message, createdAt) VALUES("
                    + std::to_string(id) + ",'" + method + "','success','Paid',"
                    + std::to_string(now) + ");";

                // [Singleton] استخدام المؤشر ->
                if (db->execute(rec)) { 
                    std::cout << "Receipt recorded.\n";
                } else {
                    std::cerr << "Warning: Payment succeeded, but receipt recording failed.\n";
                }
            } else {
                std::cout << "Could not mark appointment as paid (DB error or transaction failed).\n";
            }
        } else {
            // 1. فشل الدفع
            std::cout << "Payment failed: declined or invalid details.\n";

            // 2. تسجيل الإيصال (الفشل)
            std::string rec =
                "INSERT INTO receipts(appointmentId, method, status, message, createdAt) VALUES("
                + std::to_string(id) + ",'" + method + "','failed','Declined',"
                + std::to_string(now) + ");";

            // [Singleton] استخدام المؤشر ->
            db->execute(rec);
        }
        
    } catch (const std::exception& e) {
        // معالجة أي خطأ حرج يحدث أثناء عمليات قاعدة البيانات
        std::cerr << "CRITICAL EXCEPTION CAUGHT (PaymentCLI): DB operation failed: " << e.what() << std::endl;
        std::cout << "Payment processing halted due to critical system error.\n";
    }
}