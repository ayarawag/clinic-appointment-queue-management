#include "queue.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <sstream>
#include <iostream>
#include <stdexcept> // لإضافة معالجة الاستثناءات

// هل الـ appointment موجود في الصف مسبقاً؟
bool Queue::exists(int appointmentId) const {
    for (auto &p : queueList) {
        if (p.first == appointmentId)
            return true;
    }
    return false;
}

// إعادة ترتيب الأرقام بحيث تكون 1,2,3,...
void Queue::normalizePositions() {
    int pos = 1;
    for (auto &p : queueList) {
        p.second = pos++;
    }
}

void Queue::addPatient(int appointmentId) {
    if (exists(appointmentId)) {
        std::cout << "Already in queue.\n";
        return;
    }

    int pos = queueList.size() + 1;
    queueList.push_back({appointmentId, pos});
    normalizePositions();
}

void Queue::reorder(int appointmentId, int newPos) {
    // التحقق من صلاحية الموقع الجديد
    if (newPos <= 0 || newPos > queueList.size()) {
        newPos = queueList.size();
    }

    // نلقى العنصر
    for (size_t i = 0; i < queueList.size(); i++) {
        if (queueList[i].first == appointmentId) {
            // نشيله من مكانه
            auto item = queueList[i];
            queueList.erase(queueList.begin() + i);

            // ندخلو في المكان الجديد
            queueList.insert(queueList.begin() + (newPos - 1), item);

            break;
        }
    }

    normalizePositions();
}

// ==========================================================
// الدالة refreshPositionsDB: تطبيق Singleton و Try/Catch
// ==========================================================
void Queue::refreshPositionsDB(const std::string& dbfile) {
    try {
        // [Singleton] استخدام getInstance للحصول على نسخة الاتصال الوحيدة
        DBConnection* conn = DBConnection::getInstance(dbfile);

        for (auto &p : queueList) {
            std::ostringstream q;
            q << "UPDATE appointments SET queue_position=" << p.second
              << " WHERE id=" << p.first;

            // [Singleton] استخدام المؤشر -> لتنفيذ الاستعلام
            conn->execute(q.str());
        }
        
    } catch (const std::exception& e) {
        // [Try/Catch] معالجة أي استثناء (مثل فشل الاتصال)
        std::cerr << "EXCEPTION CAUGHT (RefreshQueue): Failed to update DB: " << e.what() << std::endl;
        // يمكننا هنا إضافة منطق لمعالجة الفشل (مثل محاولة إعادة الاتصال)
    }
}

std::vector<std::pair<int,int>> Queue::getCurrentQueue() const {
    return queueList;
}