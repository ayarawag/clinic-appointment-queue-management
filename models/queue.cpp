#include "queue.h"
#include <algorithm> // لعمليات الترتيب

using namespace std;

// المنشئ الافتراضي
Queue::Queue() {}

/**
 * @brief تضيف مريضاً إلى نهاية قائمة الانتظار.
 * @param patientId رقم تعريف المريض.
 */
void Queue::addPatient(int patientId) {
    // 1. حساب الموضع الجديد (نهاية القائمة)
    int newPosition = queueItems.empty() ? 1 : queueItems.back().position + 1;
    
    // 2. إضافة المريض إلى القائمة في الذاكرة
    QueueItem newItem = {patientId, newPosition};
    queueItems.push_back(newItem);
    
    cout << "Patient " << patientId << " added at position " << newPosition << endl;
}

/**
 * @brief تزيل مريضاً من قائمة الانتظار.
 * @param patientId رقم تعريف المريض.
 */
void Queue::removePatient(int patientId) {
    // استخدام erase-remove idiom لإزالة العنصر من الـ vector
    auto it = std::remove_if(queueItems.begin(), queueItems.end(), 
        [patientId](const QueueItem& item){
            return item.patientId == patientId;
        });

    if (it != queueItems.end()) {
        queueItems.erase(it, queueItems.end());
        cout << "Patient " << patientId << " removed from the queue." << endl;
        // بعد الإزالة، يجب تحديث مواضع جميع المرضى الذين كانوا بعده
        refreshPositionsDB(); 
    } else {
        cout << "Patient " << patientId << " not found in the queue." << endl;
    }
}

/**
 * @brief تعيد ترتيب مريض إلى موضع جديد.
 * @param patientId رقم تعريف المريض.
 * @param newPosition الموضع الجديد المطلوب.
 */
void Queue::reorder(int patientId, int newPosition) {
    auto it = std::find_if(queueItems.begin(), queueItems.end(), 
        [patientId](const QueueItem& item){
            return item.patientId == patientId;
        });

    if (it == queueItems.end()) {
        cout << "Error: Patient " << patientId << " not found for reorder." << endl;
        return;
    }

    // 1. حفظ بيانات المريض المراد نقله
    QueueItem movedItem = *it;
    
    // 2. إزالة المريض من مكانه الحالي
    queueItems.erase(it);

    // 3. تحديد الموضع الجديد (مع الأخذ في الاعتبار حدود القائمة)
    size_t targetIndex = std::min((size_t)newPosition - 1, queueItems.size());
    
    // 4. إدراج المريض في الموضع الجديد
    queueItems.insert(queueItems.begin() + targetIndex, movedItem);

    cout << "Patient " << patientId << " moved to new position " << newPosition << endl;
}

/**
 * @brief ترجع الموضع الحالي للمريض.
 * @param patientId رقم تعريف المريض.
 * @return الموضع (يبدأ من 1) أو 0 إذا لم يتم العثور عليه.
 */
int Queue::getCurrentPosition(int patientId) const {
    for (size_t i = 0; i < queueItems.size(); ++i) {
        if (queueItems[i].patientId == patientId) {
            return (int)i + 1; // المواقع تبدأ من 1
        }
    }
    return 0; // لم يتم العثور عليه
}

/**
 * @brief تحديث المواقع في قاعدة البيانات بناءً على الترتيب الحالي في الذاكرة.
 * (هذا التطبيق مجرد هيكل، يحتاج إلى كود SQLite لاحقاً).
 */
bool Queue::refreshPositionsDB() {
    // هذا الجزء يحتاج إلى تطبيق فعلي للاتصال بقاعدة البيانات وتحديث عمود "position" لكل مريض.
    // حالياً، سنقوم بتحديث المواقع في الذاكرة فقط لتجنب أخطاء الربط.
    for (size_t i = 0; i < queueItems.size(); ++i) {
        queueItems[i].position = (int)i + 1;
    }

    // [attachment_0](attachment)

    // هنا يتم تطبيق كود SQLite لتحديث جدول Queue
    cout << "Queue positions refreshed in memory and ready for DB update." << endl;
    return true; 
}

/**
 * @brief عرض قائمة الانتظار الحالية.
 */
void Queue::viewQueue() {
    cout << "\n--- Current Queue ---\n";
    if (queueItems.empty()) {
        cout << "The queue is empty.\n";
        return;
    }
    for (const auto& item : queueItems) {
        cout << "Position " << item.position << ": Patient ID " << item.patientId << endl;
    }
    cout << "---------------------\n";
}