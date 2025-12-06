#include "gtest/gtest.h"
#include "../models/notification.h"   // هذا الملف يغطي الآن جميع الدوال (logNotification, sendNotification, etc.)
#include "../database/db_connection.h"
#include <sstream>
#include <string>

using namespace std;

// تعريفات اختبارية
#define TEST_USER_ID 999
#define TEST_MESSAGE "Your appointment is confirmed."

// كلاس الإعداد (Setup) لتنظيف قاعدة البيانات قبل كل اختبار
class NotificationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // تنظيف الجدول notifications قبل كل اختبار
        DBConnection db("../../clinic.db");
        db.execute("DELETE FROM notifications;");
    }
};

// =========================================================
// اختبار الدوال المتعلقة بالموديل (models/notification.cpp)
// =========================================================

// اختبار تسجيل إشعار بنجاح
TEST_F(NotificationTest, TestLogNotificationSuccess) {
    bool success = logNotification(TEST_USER_ID, TEST_MESSAGE);
    ASSERT_TRUE(success) << "Logging notification failed.";
    
    int count = getNotificationCount(TEST_USER_ID);
    ASSERT_EQ(count, 1) << "Expected count 1 after logging, got " << count;
}

// اختبار تسجيل إشعارات متعددة واسترجاعها
TEST_F(NotificationTest, TestGetNotificationsAndMarkAsRead) {
    logNotification(TEST_USER_ID, "Message 1");
    logNotification(TEST_USER_ID, "Message 2");
    
    ASSERT_EQ(getNotificationCount(TEST_USER_ID), 2); 

    vector<string> messages = getNotificationsForUser(TEST_USER_ID);

    ASSERT_EQ(messages.size(), 2) << "Expected 2 messages, got " << messages.size();
    
    int countAfterRead = getNotificationCount(TEST_USER_ID);
    ASSERT_EQ(countAfterRead, 0) << "Expected count 0 after reading, got " << countAfterRead;
}

// =========================================================
// اختبار الدوال المتعلقة بالخاصية (features/notification.cpp)
// =========================================================

// اختبار وظيفة sendNotification والتحقق من الخرج (cout)
TEST(NotificationFeatureTest, TestSendNotificationOutput) {
    // اعتراض الخرج
    stringstream output;
    streambuf* oldCout = cout.rdbuf();
    cout.rdbuf(output.rdbuf());

    sendNotification(100, "Your meeting is tomorrow."); 

    // إعادة الخرج الطبيعي
    cout.rdbuf(oldCout); 

    string actualOutput = output.str();
    
    ASSERT_TRUE(actualOutput.find("Notify (via") != string::npos) 
        << "Output check failed. Got: " << actualOutput;

    ASSERT_TRUE(actualOutput.find("Your meeting is tomorrow.") != string::npos) 
        << "Message text not found in output. Got: " << actualOutput;
}

// اختبار وظيفة runReminders والتحقق من الخرج
TEST(NotificationFeatureTest, TestRunRemindersOutput) {
    // اعتراض الخرج
    stringstream output;
    streambuf* oldCout = cout.rdbuf();
    cout.rdbuf(output.rdbuf());

    runReminders(30); 

    // إعادة الخرج الطبيعي
    cout.rdbuf(oldCout); 

    string expectedHeader = "Reminder: (simple demo) listing all appointments:\n";
    string actualOutput = output.str();

    ASSERT_TRUE(actualOutput.find(expectedHeader) != string::npos)
        << "Expected reminder header, but got: " << actualOutput;
}