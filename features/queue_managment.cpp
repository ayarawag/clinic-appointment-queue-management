#include "../models/queue.h"
#include <iostream>
using namespace std;

// قائمة انتظار عالمية مؤقتة (لغرض هذا الملف)
static Queue globalQueue; 

/**
 * @brief تقوم بإضافة مريض جديد إلى قائمة الانتظار.
 */
void updateQueue() {
    int pid;
    
    cout << "=========================\n";
    cout << "   إضافة إلى قائمة الانتظار\n";
    cout << "=========================\n";
    
    cout << "رقم تعريف المريض المراد إضافته (Patient ID): "; 
    if (!(cin >> pid)) {
        cout << "إدخال غير صحيح. يجب أن يكون رقمًا.\n";
        cin.clear(); // مسح علامات الخطأ
        cin.ignore(10000, '\n'); // تجاهل ما تبقى في المدخلات
        return;
    }
    
    globalQueue.addPatient(pid);
    // نفترض أن دالة refreshPositionsDB تقوم بتحديث الترتيب في DB
    globalQueue.refreshPositionsDB(); 
    cout << "تمت إضافة المريض بنجاح إلى قائمة الانتظار.\n";
}

/**
 * @brief تقوم بإعادة ترتيب مريض في قائمة الانتظار إلى موضع جديد.
 */
void reorderQueue() {
    int pid, pos;
    
    cout << "=========================\n";
    cout << "   إعادة ترتيب قائمة الانتظار\n";
    cout << "=========================\n";
    
    cout << "رقم تعريف المريض المراد نقله (Patient ID): "; 
    if (!(cin >> pid)) {
        cout << "إدخال غير صحيح. يجب أن يكون رقمًا.\n";
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }
    
    cout << "الموضع الجديد (New position): ";
    if (!(cin >> pos)) {
        cout << "إدخال غير صحيح. يجب أن يكون رقمًا.\n";
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }
    
    globalQueue.reorder(pid, pos); 
    globalQueue.refreshPositionsDB();
    cout << "تمت إعادة ترتيب قائمة الانتظار بنجاح.\n";
}