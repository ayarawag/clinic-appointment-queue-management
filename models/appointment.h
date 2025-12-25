#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
#include <vector>   // ⭐ جديد: للبحث وإرجاع أكثر من موعد

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

    // الدوال الأساسية (بدون أي تغيير)
    bool book(const std::string& db = "clinic.db");
    
    // [تعديل سابق]: cancel تحدث الحالة بدلاً من الحذف
    bool cancel(const std::string& db = "clinic.db"); 
    
    bool reschedule(const std::string& newDateTime, const std::string& db = "clinic.db");
    bool setPaid(bool paidValue = true, const std::string& db = "clinic.db");

    static Appointment loadById(int id, const std::string& db = "clinic.db");

    // Feature: Search Appointments
    static std::vector<Appointment> search(
        int patientId,
        const std::string& date = "",   // optional
        int doctorId = 0,               // optional
        const std::string& db = "clinic.db"
    );
};

#endif