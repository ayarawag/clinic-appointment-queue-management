#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <string>
#include <vector>

using namespace std;

// Class Definition (Model)
class Notification {
public:
    int id;
    int patientId;
    string message;
    string dateTime;
    int isRead; // 0 for unread, 1 for read

    // Constructor
    Notification(int id = 0, int patientId = 0, const string& message = "", 
                 const string& dateTime = "", int isRead = 0)
        : id(id), patientId(patientId), message(message), 
          dateTime(dateTime), isRead(isRead) {}

    // Simple display function
    void display() const;
};

// Free Functions (for DB interaction)
// Function to log a new notification to the database table
bool logNotification(int userId, const string& msg);

// Function to retrieve the count of unread notifications
int getNotificationCount(int userId);

// Function to retrieve all notifications for a user
vector<string> getNotificationsForUser(int userId);

#endif // NOTIFICATION_H