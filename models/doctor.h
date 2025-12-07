#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
using namespace std;

class Doctor {
public:
    int id;
    string name; 
    string specialization; 
    string schedule;
    
    void setSchedule(const string& schedule);
    // أضفت دالة مساعدة لتمثيل جلب الطبيب بواسطة الـ ID
    bool getDoctorById(int doctorId); 
};

#endif