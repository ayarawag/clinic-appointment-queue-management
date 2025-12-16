#include <gtest/gtest.h>
#include "../models/appointment.h"
#include "../models/patient.h"
#include "../models/doctor.h"
#include "../database/db_connection.h" // كلاس DBConnection أصبح Singleton
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

// اسم قاعدة البيانات المستخدمة في الاختبارات
const std::string TEST_DB = "test_clinic.db";

// 1. دالة مساعدة لتهيئة قاعدة البيانات (تحديث Singleton)
inline void initialize_test_db(const std::string& db_name) {
    std::remove(db_name.c_str()); 
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();
        
        DBConnection* db = DBConnection::getInstance(db_name); 
        db->execute(sql_script);
    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}

// 2. دالة محاكاة الدفع (كما في payment.cpp)
bool simulatePayment(const std::string& details) {
    if (details.empty()) return false;
    char last = details.back();
    if (!isdigit(last)) return false;
    int d = last - '0';
    return (d % 2 == 0); // أرقام زوجية = قبول
}

// 3. دالة مساعدة للتحقق من حالة الدفع (تحديث Singleton)
bool isAppointmentPaid(int id, const std::string& db) {
    int paid_status = 0;
    
    try {
        DBConnection* conn = DBConnection::getInstance(db);
        std::string q = "SELECT paid FROM appointments WHERE id=" + std::to_string(id) + " LIMIT 1;";

        conn->query(q,
            [](void* data, int argc, char** argv, char** col_names) -> int {
                if (argv[0]) {
                    try {
                        *((int*)data) = std::stoi(argv[0]);
                    } catch (...) {}
                }
                return 0;
            },
            &paid_status
        );
        return (paid_status == 1);

    } catch (const std::exception& e) {
        std::cerr << "EXCEPTION CAUGHT (isAppointmentPaid): " << e.what() << std::endl;
        return false;
    }
}


// كلاس الاختبار PaymentTests
class PaymentTests : public ::testing::Test {
protected:
    int appointmentId; 

    void SetUp() override {
        initialize_test_db(TEST_DB); 

        // 1. تسجيل مريض وطبيب (للالتزام بالقيود الخارجية)
        Patient("PayPatient", "100", "p@pay.com", "pass").registerPatient(TEST_DB); // ID 1
        Doctor("Dr. Pay", "Surgeon").registerDoctor(TEST_DB); // ID 1
        
        // 2. حجز موعد (هذا سيكون ID 1)
        Appointment a;
        a.patientId = 1;
        a.doctorId = 1;
        a.dateTime = "2026-05-10 11:00:00";
        a.book(TEST_DB);
        
        ASSERT_TRUE(a.id != 0);
        appointmentId = a.id;
    }
    
    void TearDown() override {
        // [ملاحظة Singleton]
        // DBConnection::destroyInstance();
    }
};

// --------------------------------------------------------
// الاختبارات
// --------------------------------------------------------

// 1. اختبار عملية الدفع الناجحة وتحديث حالة الموعد
TEST_F(PaymentTests, SuccessfulPaymentAndSetPaid) {
    // "card_1234" ينتهي برقم زوجي، لذا سينجح الدفع
    bool ok = simulatePayment("card_1234");
    ASSERT_TRUE(ok);

    // *نحاكي* تحديث حالة الموعد وتسجيل الإيصال
    // *ملاحظة*: بما أننا لا يمكننا اختبار دالة CLI (payAppointmentCLI) مباشرة، 
    // يجب أن نختبر الدالة المسؤولة عن تحديث حالة الدفع في الكلاس Appointment
    
    // التحقق المبدئي: يجب أن يكون غير مدفوع
    ASSERT_FALSE(isAppointmentPaid(appointmentId, TEST_DB));

    Appointment a = Appointment::loadById(appointmentId, TEST_DB);
    bool set_paid_ok = a.setPaid(true, TEST_DB); // هذه الدالة تستخدم Singleton الآن

    // التحقق من أن عملية تحديث paid=1 نجحت
    EXPECT_TRUE(set_paid_ok);
    
    // التحقق من حالة الموعد في قاعدة البيانات
    EXPECT_TRUE(isAppointmentPaid(appointmentId, TEST_DB)) 
        << "Appointment status in DB should be paid (1).";
}


// 2. اختبار عملية الدفع الفاشلة (بافتراض أن setPaid لم تُستدعَ)
TEST_F(PaymentTests, FailedPaymentLogic) {
    // "card_1235" ينتهي برقم فردي، لذا سيفشل الدفع
    bool ok = simulatePayment("card_1235");
    ASSERT_FALSE(ok);

    // للتأكد من أن حالة الدفع لم تتغير (لأن setPaid لم تُستدعَ)
    EXPECT_FALSE(isAppointmentPaid(appointmentId, TEST_DB)) 
        << "Appointment status in DB should remain unpaid (0).";
    
    // إذا كان منطق الاختبار يقتضي التأكد من تسجيل الإيصال بحالة 'failed'
    // يجب إضافة دالة مساعدة لتحميل الإيصالات والتحقق من محتواها، لكننا نكتفي بمنطق الموعد هنا.
}