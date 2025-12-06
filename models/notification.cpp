#include "notification.h"
#include "../database/db_connection.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

using namespace std;

// Simple display function implementation
void Notification::display() const {
    cout << "ID: " << id 
         << " | Patient: " << patientId 
         << " | Message: " << message.substr(0, 40) << "..." 
         << " | Time: " << dateTime 
         << " | Read: " << (isRead ? "Yes" : "No") << endl;
}

// Helper function to get current date and time
string getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    stringstream ss;
    ss << (1900 + ltm->tm_year) << "-";
    ss << setfill('0') << setw(2) << (1 + ltm->tm_mon) << "-";
    ss << setfill('0') << setw(2) << ltm->tm_mday << " ";
    ss << setfill('0') << setw(2) << ltm->tm_hour << ":";
    ss << setfill('0') << setw(2) << ltm->tm_min << ":";
    ss << setfill('0') << setw(2) << ltm->tm_sec;
    
    return ss.str();
}

// Helper function to sanitize message (simple quote replacement)
string sanitizeMsg(string msg) {
    // Replace single quotes to prevent basic SQL injection
    replace(msg.begin(), msg.end(), '\'', '"');
    return msg;
}

// === Log a new notification ===
bool logNotification(int userId, const string& msg) {
    string sanitizedMsg = sanitizeMsg(msg);
    string currentTime = getCurrentDateTime();
    
    // Path assumption: from models/ to database/
    DBConnection db("../../clinic.db"); 
    
    string sql = "INSERT INTO notifications (patientId, message, dateTime, isRead) VALUES (" +
                 to_string(userId) + ", '" + sanitizedMsg + "', '" + currentTime + "', 0);"; // 0 for unread

    if (db.execute(sql)) {
        return true;
    } else {
        cerr << "Error logging notification." << endl;
        return false;
    }
}

// === Get count of unread notifications ===
int getNotificationCount(int userId) {
    DBConnection db("../../clinic.db");
    int count = 0;
    
    string sql = "SELECT COUNT(*) FROM notifications WHERE patientId = " + to_string(userId) + " AND isRead = 0;";
    
    // الملاحظة الهامة: لم يعد هناك التقاط للمتغيرات [&]
    // نمرر عنوان count عبر الوسيط الأخير (&count)
    db.query(sql, 
        [](void* data, int argc, char** vals, char** cols) -> int {
            int* countPtr = static_cast<int*>(data); // تحويل المؤشر
            if (vals[0] && countPtr) {
                *countPtr = stoi(vals[0]); // تحديث قيمة count عبر المؤشر
            }
            return 0;
        }, &count); // تمرير عنوان count

    return count;
}

// === Retrieve notifications and mark as read ===
vector<string> getNotificationsForUser(int userId) {
    DBConnection db("../../clinic.db");
    vector<string> messages;
    
    string sql = "SELECT message FROM notifications WHERE patientId = " + to_string(userId) + " ORDER BY dateTime DESC;";
    
    // الملاحظة الهامة: لم يعد هناك التقاط للمتغيرات [&]
    // نمرر عنوان messages عبر الوسيط الأخير (&messages)
    db.query(sql, 
        [](void* data, int argc, char** vals, char** cols) -> int {
            vector<string>* messagesPtr = static_cast<vector<string>*>(data); // تحويل المؤشر
            if (vals[0] && messagesPtr) {
                messagesPtr->push_back(vals[0]); // إضافة الرسالة للمتجه عبر المؤشر
            }
            return 0;
        }, &messages); // تمرير عنوان messages

    // Update notifications status to read
    string updateSql = "UPDATE notifications SET isRead = 1 WHERE patientId = " + to_string(userId) + ";";
    db.execute(updateSql);

    return messages;
}