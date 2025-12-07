#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
using namespace std;

class Appointment {
public:
    int id, patientId, doctorId;
    string dateTime;
    bool paid;

    bool book();
    bool cancel();
    bool reschedule(const string& newTime);
    bool setPaid();
    // دالة لاسترجاع موعد بواسطة الـ ID
    bool getAppointmentById(int appointmentId); 
};

#endif