#include <gtest/gtest.h> 
#include "../models/notification.h"
#include "../models/patient.h"
#include "../models/doctor.h"
#include "../models/appointment.h" 
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept> // لإضافة معالجة الاستثناءات
#include <iostream>  // لـ std::cerr

// اسم قاعدة البيانات التي سيتم استخدامها في الاختبارات فقط
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (مع تحديث Singleton)
inline void initialize_test_db(const std::string& db_name) {
    std::remove(db_name.c_str()); 
    
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        // [تعديل Singleton] استخدام getInstance بدلاً من المُنشئ
        DBConnection* db = DBConnection::getInstance(db_name); 
        
        // [تعديل Singleton] استخدام المؤشر -> لتنفيذ الدالة
        db->execute(sql_script);
        
        // يمكننا تدمير النسخة هنا إذا أردنا (DBConnection::destroyInstance();)
    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}

// دالة مساعدة لتحميل عدد الإشعارات من قاعدة البيانات (مع تحديث Singleton)
int getNotificationCount(const std::string& db_name) {
    
    // [تعديل Singleton] استخدام getInstance بدلاً من المُنشئ
    DBConnection* conn = DBConnection::getInstance(db_name); 
    int count = 0;
    
    // [تعديل Singleton] استخدام المؤشر -> للاستعلام
    conn->query(
        "SELECT COUNT(*) FROM notifications;",
        [](void* data, int argc, char** argv, char** col_names) -> int {
            int* count_ptr = static_cast<int*>(data);
            if (argv[0]) {
                // يجب أن تكون هذه العملية محمية بـ try/catch في كود الإنتاج
                try {
                    *count_ptr = std::stoi(argv[0]);
                } catch (const std::exception& e) {
                    // إذا فشل التحويل
                }
            }
            return 0;
        },
        &count
    );
    // [تعديل Singleton] قد تحتاج هنا إلى DBConnection::destroyInstance();
    return count;
}


// كلاس الاختبار NotificationTests
class NotificationTests : public ::testing::Test {
protected:
    void SetUp() override {
        initialize_test_db(TEST_DB); 
        
        // 1. تسجيل مريض 
        // يجب التأكد أن registerPatient تستخدم Singleton
        Patient("NotifyPat", "100", "p@notify.com", "pass").registerPatient(TEST_DB); // ID 1
        
        // 2. تسجيل طبيب 
        // يجب التأكد أن registerDoctor تستخدم Singleton
        Doctor("Dr. Notify", "Generalist").registerDoctor(TEST_DB); // ID 1
        
        // 3. موعد للاختبار 
        Appointment a_future;
        a_future.patientId = 1;
        a_future.doctorId = 1;
        a_future.dateTime = "2025-12-17 10:00:00"; 
        // يجب التأكد أن book تستخدم Singleton
        a_future.book(TEST_DB); 
    }
    
    void TearDown() override {
        // إذا كان لديك دالة لتدمير نسخة Singleton في DBConnection، يجب استدعاؤها هنا
        // DBConnection::destroyInstance();
    }
};

// --------------------------------------------------------
// الاختبارات (تبقى كما هي)
// --------------------------------------------------------

TEST_F(NotificationTests, ConstructorAndInitialState) {
    Notification n_create(5, "Your appointment is tomorrow.");
    
    EXPECT_EQ(n_create.patientId, 5);
    EXPECT_EQ(n_create.message, "Your appointment is tomorrow.");
}

TEST_F(NotificationTests, SendNotificationSuccess) {
    // 1. التحقق من أن عدد الإشعارات المبدئي صفر
    int initial_count = getNotificationCount(TEST_DB);
    EXPECT_EQ(initial_count, 0);
    
    // 2. إرسال إشعار
    Notification n_send(1, "Reminder test message");
    bool ok_send = n_send.sendNotification(TEST_DB);

    EXPECT_TRUE(ok_send);
    
    // 3. التحقق من أن عدد الإشعارات زاد
    int final_count = getNotificationCount(TEST_DB);
    EXPECT_EQ(final_count, 1);
}

TEST_F(NotificationTests, RunRemindersPlaceholder) {
    bool ok_run = true;
    
    try {
        Notification::runReminders(TEST_DB); 
    } catch (...) {
        ok_run = false;
    }

    EXPECT_TRUE(ok_run);
}