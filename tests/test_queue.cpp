#include <gtest/gtest.h>
#include "../models/queue.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <fstream>
#include <sstream>
#include <algorithm> 
#include <iostream> 
#include <stdexcept> 

// اسم قاعدة البيانات التي سيتم استخدامها في الاختبارات فقط
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (مع تحديث Singleton)
inline void initialize_test_db(const std::string& db_name) {
    // 1. حذف الملف القديم لضمان بداية نظيفة
    std::remove(db_name.c_str()); 
    
    // 2. قراءة محتوى سكريبت SQL
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        // 3. فتح قاعدة البيانات وتنفيذ السكريبت
        // [تعديل Singleton] استخدام getInstance بدلاً من المُنشئ
        DBConnection* db = DBConnection::getInstance(db_name); 
        
        // [تعديل Singleton] استخدام المؤشر -> لتنفيذ الدالة
        db->execute(sql_script);
        
        // [ملاحظة Singleton] هنا يجب أن نفكر في استدعاء DBConnection::destroyInstance();
    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}


// كلاس الاختبار QueueTests
class QueueTests : public ::testing::Test {
protected:
    Queue q;
    
    void SetUp() override {
        initialize_test_db(TEST_DB); 
    }
    
    void TearDown() override {
        // [ملاحظة Singleton] يجب استدعاء دالة التدمير هنا إذا كانت موجودة لضمان عزل الاختبارات
        // DBConnection::destroyInstance();
    }
    
    // دالة مساعدة للتحقق من موقع مريض معين (لا تحتاج لتعديل Singleton)
    int getPatientPosition(int patient_id, const std::vector<std::pair<int, int>>& queue_list) {
        for (const auto& p : queue_list) {
            if (p.first == patient_id) {
                return p.second; // تعيد الموقع
            }
        }
        return -1; // لم يتم العثور عليه
    }
};

// --------------------------------------------------------
// الاختبارات
// --------------------------------------------------------

TEST_F(QueueTests, AddAndOrder) {
    // إضافة مرضى
    q.addPatient(10); // Pos 1
    q.addPatient(20); // Pos 2
    q.addPatient(30); // Pos 3

    auto list = q.getCurrentQueue();

    // التحقق من الحجم
    EXPECT_EQ(list.size(), 3);
    
    // التحقق من الترتيب والموقع
    EXPECT_EQ(getPatientPosition(10, list), 1);
    EXPECT_EQ(getPatientPosition(20, list), 2);
    EXPECT_EQ(getPatientPosition(30, list), 3);
}

TEST_F(QueueTests, Reorder) {
    // 1. إعداد قائمة
    q.addPatient(10); // pos 1
    q.addPatient(20); // pos 2
    q.addPatient(30); // pos 3

    // 2. إعادة ترتيب المريض 20 ليصبح في الموقع 1 (يجب أن يتم دفع 10 إلى الموقع 2)
    q.reorder(20, 1);

    auto list = q.getCurrentQueue();

    // 3. التحقق من الترتيب الجديد
    EXPECT_EQ(getPatientPosition(20, list), 1);
    EXPECT_EQ(getPatientPosition(10, list), 2);
    EXPECT_EQ(getPatientPosition(30, list), 3);
}

TEST_F(QueueTests, RefreshDB) {
    // 1. إضافة مرضى إلى القائمة في الذاكرة
    q.addPatient(1);
    q.addPatient(2);

    // 2. تحديث قاعدة البيانات (يجب أن تنجح دون طرح استثناءات)
    bool ok = true;
    try {
        // هذه الدالة (refreshPositionsDB) تستخدم Singleton و Try/Catch الآن
        q.refreshPositionsDB(TEST_DB); 
    } catch (...) {
        ok = false;
    }

    EXPECT_TRUE(ok);
}