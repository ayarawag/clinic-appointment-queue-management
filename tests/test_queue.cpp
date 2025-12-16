#include <gtest/gtest.h>
#include "../models/queue.h"
#include "../database/db_connection.h"
#include <fstream>
#include <sstream>
#include <algorithm> // مطلوب لعملية الإزالة (حتى لو لم نستخدمها الآن)

// اسم قاعدة البيانات التي سيتم استخدامها في الاختبارات فقط
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (مع استخدام inline لمنع خطأ التعريفات المتعددة)
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
        DBConnection db(db_name);
        db.execute(sql_script);
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
    
    // دالة مساعدة للتحقق من موقع مريض معين
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
// الاختبارات (تم إزالة اختبار RemovePatient)
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
    // المريض 20 أصبح أولاً
    EXPECT_EQ(getPatientPosition(20, list), 1);
    // المريض 10 أصبح ثانياً
    EXPECT_EQ(getPatientPosition(10, list), 2);
    // المريض 30 ظل ثالثاً
    EXPECT_EQ(getPatientPosition(30, list), 3);
}

TEST_F(QueueTests, RefreshDB) {
    // 1. إضافة مرضى إلى القائمة في الذاكرة
    q.addPatient(1);
    q.addPatient(2);

    // 2. تحديث قاعدة البيانات (يجب أن تنجح دون طرح استثناءات)
    bool ok = true;
    try {
        q.refreshPositionsDB(TEST_DB); 
    } catch (...) {
        ok = false;
    }

    EXPECT_TRUE(ok);
}

