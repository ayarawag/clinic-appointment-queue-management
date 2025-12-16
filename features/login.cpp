#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include "../utils/password_utils.h"
#include <iostream>
#include <string>
#include <stdexcept> // لإضافة معالجة الاستثناءات

bool loginUser() {
    std::string email, password;

    std::cout << "Enter your email: ";
    // استخدام getline لتجنب مشاكل المسافات في الإدخال، لكن سنبقي على cin لسهولة CLI
    std::cin >> email; 

    std::cout << "Enter your password: ";
    std::cin >> password;

    // التحقق من المدخلات الأساسية
    if (email.empty() || password.empty()) {
        std::cout << "Email and password cannot be empty.\n";
        return false;
    }

    std::string storedHash = "";
    const std::string DB_FILE = "clinic.db";

    try {
        // [Singleton] الحصول على نسخة الاتصال الوحيدة
        DBConnection* db = DBConnection::getInstance(DB_FILE); 
        
        // الاستعلام الصحيح
        std::string q = 
            "SELECT password_hash FROM patients WHERE email='" + email + "' LIMIT 1;";

        // [Singleton] استخدام المؤشر -> لقراءة نتيجة الاستعلام
        db->query(q,
            [](void* out, int cols, char** vals, char**) -> int {
                if (vals[0]) {
                    // يجب أن يكون التحويل آمناً (نحن نقرأ string)
                    *((std::string*)out) = vals[0];
                }
                return 0;
            },
            &storedHash
        );

        if (storedHash.empty()) {
            std::cout << "Login failed: Email not found.\n";
            return false;
        }

        // مقارنة كلمة السر (لا تتفاعل مع DB)
        if (PasswordUtils::verifyPassword(password, storedHash)) {
            std::cout << "Login successful.\n";
            return true;
        } else {
            std::cout << "Login failed: Invalid password.\n";
            return false;
        }

    } catch (const std::exception& e) {
        // [Try/Catch] معالجة أي استثناء (مثل فشل الاتصال بقاعدة البيانات)
        std::cerr << "CRITICAL ERROR: Failed to process login due to DB issue: " << e.what() << std::endl;
        std::cout << "Login processing failed due to system error.\n";
        return false;
    }
}