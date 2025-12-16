#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <iostream>
#include <string>
#include <stdexcept> // لإضافة معالجة الاستثناءات

void viewAppointmentsCLI() {
    
    const std::string DB_FILE = "clinic.db";
    
    // [Singleton] الحصول على نسخة الاتصال الوحيدة
    DBConnection* db = DBConnection::getInstance(DB_FILE); 
    
    std::cout << "\n--- All Appointments ---\n";

    try {
        
        // [Singleton] استخدام المؤشر -> للاستعلام
        db->query("SELECT id, patientId, doctorId, dateTime, status, paid FROM appointments ORDER BY dateTime ASC;",
            // الـ Callback تبقى كما هي (تقوم فقط بالعرض)
            [](void*, int cols, char** vals, char**) -> int {
                
                // إضافة فحص بسيط لتجنب Null Pointers (مع أن vals يجب أن تكون 0 أو غير ذلك)
                std::cout << "ID:" << (vals[0] ? vals[0] : "N/A") 
                          << " | PatientID:" << (vals[1] ? vals[1] : "N/A")
                          << " | DoctorID:" << (vals[2] ? vals[2] : "N/A") 
                          << " | Time:" << (vals[3] ? vals[3] : "N/A")
                          << " | Status:" << (vals[4] ? vals[4] : "N/A") 
                          << " | Paid:" << (vals[5] ? vals[5] : "0") << "\n";
                return 0;
            }, 
            nullptr);
            
        std::cout << "--------------------------\n";

    } catch (const std::exception& e) {
        // [Try/Catch] معالجة أي استثناء (مثل فشل الاتصال بقاعدة البيانات)
        std::cerr << "CRITICAL ERROR: Failed to load appointments from DB: " << e.what() << std::endl;
        std::cout << "Could not display appointments due to a system error.\n";
    }
}