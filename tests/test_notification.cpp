#include "gtest/gtest.h"
#include "../models/notification.h"
#include "../models/patient.h" 
#include "../database/db_connection.h" 
#include <iostream>
#include <cstdio> // لـ remove

using namespace std;

// تعريف مسار قاعدة البيانات الخاص بالاختبارات
const string TEST_DB_PATH = "clinic.db";

// =======================================================
// كلاس إعداد الاختبار (Setup)
// =======================================================
class NotificationTest : public ::testing::Test {
protected:
    // هذه الدالة تُستدعى مرة واحدة قبل تشغيل أي اختبار في هذه المجموعة
    static void SetUpTestSuite() {
        // 1. حذف قاعدة البيانات القديمة لضمان بيئة نظيفة
        remove(TEST_DB_PATH.c_str());

        // 2. إنشاء اتصال وتهيئة الجداول (بما في ذلك notifications)
        sqlite3* db = connectDB(TEST_DB_PATH);
        if (db) {
            initializeDatabaseSchema(db); // يفترض وجود initializeDatabaseSchema في db_connection
            closeDB(db);
        } else {
            cerr << "Failed to connect to DB for setup." << endl;
        }
    }
};

// =======================================================
// الاختبارات الفعلية
// =======================================================

// الاختبار 1: التحقق من تسجيل الإشعار
TEST_F(NotificationTest, TestLogNotificationSuccess) {
    // 1. إنشاء مريض وهمي لضمان وجود userId صالح
    Patient p;
    p.name = "Test User";
    p.email = "notification_test@example.com";
    p.phone = "0000000000";
    p.password = "P@$$w0rd1";
    int userId = p.registerPatient(TEST_DB_PATH);
    
    // تأكد من أن المستخدم تم تسجيله بنجاح
    ASSERT_NE(userId, 0) << "Patient registration failed, cannot test notification.";

    // 2. تسجيل الإشعار (عبر كائن Notification)
    Notification n;
    // الاستدعاء الآن يحتوي على 5 حجج (dbPath, userId, message, type, status)
    bool success = n.logNotification(TEST_DB_PATH, userId, "Test message", "SMS", "PENDING");

    // 3. التحقق
    ASSERT_TRUE(success) << "Logging notification failed.";
}

// الاختبار 2: التحقق من الحصول على الإشعارات وتحديدها كمقروءة
TEST_F(NotificationTest, TestGetNotificationsAndMarkAsRead) {
    // 1. إنشاء مريض وهمي
    Patient p;
    p.name = "Read Test User";
    p.email = "read_notification@example.com";
    p.phone = "1111111111";
    p.password = "P@$$w0rd2";
    int userId = p.registerPatient(TEST_DB_PATH);

    ASSERT_NE(userId, 0) << "Patient registration failed, cannot test notification reading.";

    // 2. تسجيل إشعارين
    Notification n;
    n.logNotification(TEST_DB_PATH, userId, "Message 1", "SMS", "PENDING");
    n.logNotification(TEST_DB_PATH, userId, "Message 2", "APP", "PENDING");

    // 3. التحقق من عدد الإشعارات الجديدة
    int count = n.getNotificationCount(TEST_DB_PATH, userId, "PENDING");
    ASSERT_EQ(count, 2) << "Expected 2 unread notifications.";

    // 4. الحصول على الإشعارات
    vector<Notification> notifications = n.getNotifications(TEST_DB_PATH, userId, "PENDING");
    ASSERT_EQ(notifications.size(), 2) << "Expected to retrieve 2 notifications.";

    // 5. تحديد الإشعارات كمقروءة
    bool success = n.markNotificationsAsRead(TEST_DB_PATH, userId);
    ASSERT_TRUE(success);

    // 6. التحقق من أن العدد أصبح صفراً (PENDING)
    ASSERT_EQ(n.getNotificationCount(TEST_DB_PATH, userId, "PENDING"), 0) << "Expected PENDING count to be 0 after marking as read.";
    // 7. التحقق من أن العدد أصبح 2 (READ)
    ASSERT_EQ(n.getNotificationCount(TEST_DB_PATH, userId, "READ"), 2) << "Expected READ count to be 2 after marking as read.";
}