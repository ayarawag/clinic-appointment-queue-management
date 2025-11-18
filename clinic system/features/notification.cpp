#include <iostream>

void sendNotification(int patientId, const std::string& message) {
    std::cout << "Notification to Patient " << patientId << ": " << message << std::endl;
}