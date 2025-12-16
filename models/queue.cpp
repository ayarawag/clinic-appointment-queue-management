#include "queue.h"
#include "../database/db_connection.h" 
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>

// ==========================================================
// الدوال المساعدة (في الذاكرة)
// ==========================================================

bool Queue::exists(int appointmentId) const {
    for (const auto& item : queueList) {
        if (item.first == appointmentId) {
            return true;
        }
    }
    return false;
}

void Queue::normalizePositions() {
    int currentPos = 1;
    // فرز القائمة حسب الموقع الحالي
    std::sort(queueList.begin(), queueList.end(), 
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second < b.second;
        });

    for (auto& item : queueList) {
        item.second = currentPos++;
    }
}

// ==========================================================
// الدوال الأساسية (في الذاكرة)
// ==========================================================

void Queue::addPatient(int appointmentId) {
    if (appointmentId <= 0 || exists(appointmentId)) {
        return; 
    }
    
    int maxPos = 0;
    for (const auto& item : queueList) {
        if (item.second > maxPos) {
            maxPos = item.second;
        }
    }
    queueList.emplace_back(appointmentId, maxPos + 1);
}

// 2. الدالة reorder: تم إصلاح منطق النقل (الدفع/السحب)
void Queue::reorder(int appointmentId, int newPos) {
    if (newPos <= 0 || !exists(appointmentId)) {
        return; 
    }
    
    // البحث عن الموعد المراد نقله
    auto it = std::find_if(queueList.begin(), queueList.end(), 
        [appointmentId](const std::pair<int, int>& p) {
            return p.first == appointmentId;
        });
        
    if (it != queueList.end()) {
        int oldPos = it->second;

        if (newPos == oldPos) return; 
        
        // دفع/سحب العناصر الأخرى
        for (auto& item : queueList) {
            if (item.first != appointmentId) {
                if (newPos < oldPos && item.second >= newPos && item.second < oldPos) {
                    // النقل للأعلى: دفع العناصر للأسفل
                    item.second += 1; 
                } else if (newPos > oldPos && item.second > oldPos && item.second <= newPos) {
                    // النقل للأسفل: سحب العناصر للأعلى
                    item.second -= 1; 
                }
            }
        }
        
        // تعيين الموقع الجديد للعنصر المنقول
        it->second = newPos;
        
        // تطبيع المواقع لضمان عدم وجود فجوات والترتيب الصحيح
        normalizePositions();
    }
}

std::vector<std::pair<int,int>> Queue::getCurrentQueue() const {
    std::vector<std::pair<int, int>> sortedList = queueList;
    std::sort(sortedList.begin(), sortedList.end(), 
        [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
            return a.second < b.second;
        });
    return sortedList;
}

// ==========================================================
// 3. الدالة refreshPositionsDB: تطبيق Singleton و Try/Catch
// ==========================================================
void Queue::refreshPositionsDB(const std::string& db) {
    if (queueList.empty()) return;

    try {
        DBConnection* conn = DBConnection::getInstance(db);
        
        // 1. بدء معاملة 
        conn->execute("BEGIN TRANSACTION;");
        
        // 2. تحديث المواقع في جدول appointments
        for (const auto& item : queueList) {
            int appointmentId = item.first;
            int position = item.second;
            
            std::ostringstream q;
            q << "UPDATE appointments SET queue_position=" << position
              << " WHERE id=" << appointmentId << ";";
              
            conn->execute(q.str());
        }

        // 3. إنهاء المعاملة
        conn->execute("COMMIT;");
        
        std::cout << "Queue positions successfully refreshed in DB.\n";
        
    } catch (const std::exception& e) {
        // التراجع في حالة الفشل
        try {
            DBConnection::getInstance(db)->execute("ROLLBACK;");
        } catch (...) {}
        std::cerr << "EXCEPTION CAUGHT (RefreshDB): DB operation failed. Transaction rolled back: " << e.what() << std::endl;
        throw; 
    }
}