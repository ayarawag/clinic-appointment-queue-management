#ifndef QUEUE_H
#define QUEUE_H

#include <vector>
#include <string>
#include <iostream> 

// تعريف هيكل بيانات Item لتخزين معلومات المريض في قائمة الانتظار
struct QueueItem {
    int patientId;
    int position; 
};

class Queue {
private:
    // أعضاء الكلاس لتخزين قائمة الانتظار في الذاكرة
    std::vector<QueueItem> queueItems; 

public:
    Queue();
    
    // 1. الدالة الصحيحة لإضافة مريض (تطابق مع addPatient في features/queue_managment.cpp)
    void addPatient(int patientId);
    
    // 2. دالة إزالة مريض
    void removePatient(int patientId); // تم توحيدها من removeFromQueue
    
    // 3. دالة الحصول على الموقع الحالي
    int getCurrentPosition(int patientId) const;
    
    // 4. دالة إعادة الترتيب
    void reorder(int patientId, int newPosition);
    
    // 5. دالة تحديث المواقع في قاعدة البيانات
    bool refreshPositionsDB();
    
    // دالة إظهار قائمة الانتظار
    void viewQueue(); 
};

#endif // QUEUE_H