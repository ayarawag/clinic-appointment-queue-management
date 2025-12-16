#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>

class Appointment {
public:
    // خصائص الكلاس
    int id;
    int patientId;
    int doctorId;
    std::string dateTime;
    
    // [الحل]: إضافة متغير الحالة (Status)
    std::string status = "Booked"; // قيمة افتراضية
    
    // [الحل]: التأكد من Paid
    int paid = 0; // 0 for unpaid, 1 for paid 

    Appointment();
    Appointment(int pid, int did, std::string datetime);

    // الدوال الأساسية (مع بقاء القيمة الافتراضية لملف DB)
    bool book(const std::string& db = "clinic.db");
    
    // [تعديل]: دالة cancel ستحدث الحالة بدلاً من حذف الصف
    bool cancel(const std::string& db = "clinic.db"); 
    
    bool reschedule(const std::string& newDateTime, const std::string& db = "clinic.db");
    bool setPaid(bool paidValue = true, const std::string& db = "clinic.db");

    static Appointment loadById(int id, const std::string& db = "clinic.db");
};

#endif