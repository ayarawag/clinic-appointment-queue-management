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
    DBConnection::destroyInstance(); 
    
    std::remove(db_name.c_str()); 
    
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        DBConnection* db = DBConnection::getInstance(db_name); 
        db->execute(sql_script);
        
        DBConnection::destroyInstance();
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
        DBConnection::destroyInstance();
    }
    
    // دالة مساعدة للتحقق من موقع مريض معين 
    int getPatientPosition(int patient_id, const std::vector<std::pair<int, int>>& queue_list) {
        for (const auto& p : queue_list) {
            if (p.first == patient_id) {
                return p.second; 
            }
        }
        return -1; 
    }
};

// --------------------------------------------------------
// الاختبارات
// --------------------------------------------------------

TEST_F(QueueTests, AddAndOrder) {
    q.addPatient(10); 
    q.addPatient(20); 
    q.addPatient(30); 

    auto list = q.getCurrentQueue();

    EXPECT_EQ(list.size(), 3);
    
    EXPECT_EQ(getPatientPosition(10, list), 1);
    EXPECT_EQ(getPatientPosition(20, list), 2);
    EXPECT_EQ(getPatientPosition(30, list), 3);
}

TEST_F(QueueTests, Reorder) {
    q.addPatient(10); // 1
    q.addPatient(20); // 2
    q.addPatient(30); // 3

    // نقل 20 إلى الموقع 1 (يجب أن يدفع 10 إلى الموقع 2)
    q.reorder(20, 1);

    auto list = q.getCurrentQueue();

    // الترتيب المتوقع: (20, 1), (10, 2), (30, 3)
    EXPECT_EQ(getPatientPosition(20, list), 1);
    EXPECT_EQ(getPatientPosition(10, list), 2);
    EXPECT_EQ(getPatientPosition(30, list), 3);
}

TEST_F(QueueTests, RefreshDB) {
    q.addPatient(1);
    q.addPatient(2);

    bool ok = true;
    try {
        q.refreshPositionsDB(TEST_DB); 
    } catch (...) {
        ok = false;
    }

    EXPECT_TRUE(ok);
}