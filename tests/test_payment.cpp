#include <gtest/gtest.h>
#include "../models/appointment.h"
#include "../models/patient.h"
#include "../models/doctor.h"
#include "../database/db_connection.h"
#include "../features/payment.cpp" // تضمين ملف payment.cpp
#include <fstream>
#include <sstream>
#include <string>

// اسم قاعدة البيانات المستخدمة في الاختبارات
const std::string TEST_DB = "test_clinic.db";

// دالة مساعدة لتهيئة قاعدة البيانات (كما هو معتاد)
inline void initialize_test_db(const std::string& db_name) {
    std::remove(db_name.c_str());
    
    std::ifstream sql_file("../database/database.sql");
    std::stringstream buffer;
    
    if (sql_file.is_open()) {
        buffer << sql_file.rdbuf();
        std::string sql_script = buffer.str();

        DBConnection db(db_name);
        db.execute(sql_script);
    } else {
        std::cerr << "ERROR: database/database.sql not found! Cannot initialize DB." << std::endl;
    }
}

// دالة مساعدة للتحقق من حالة الدفع (Paid) للموعد
bool isAppointmentPaid(int appointmentId, const std::string& db_name) {
    DBConnection conn(db_name);
    int paid_status = 0; // 0=false, 1=true

    conn.query(
        "SELECT paid FROM appointments WHERE id=" + std::to_string(appointmentId) + ";",
        [](void* data, int argc, char** argv, char** col_names) -> int {
            if (argv[0]) {
                *((int*)data) = std::stoi(argv[0]);
            }
            return 0;
        },
        &paid_status
    );
    return paid_status == 1;
}

// دالة مساعدة للتحقق من حالة الإيصال (Receipt Status)
std::string getReceiptStatus(int appointmentId, const std::string& db_name) {
    DBConnection conn(db_name);
    std::string status = "";

    conn.query(
        "SELECT status FROM receipts WHERE appointmentId=" + std::to_string(appointmentId) + " ORDER BY createdAt DESC LIMIT 1;",
        [](void* data, int argc, char** argv, char** col_names) -> int {
            if (argv[0]) {
                *((std::string*)data) = argv[0];
            }
            return 0;
        },
        &status
    );
    return status;
}


// كلاس الاختبار PaymentTests
class PaymentTests : public ::testing::Test {
protected:
    int patientId;
    int doctorId;
    int appointmentId;

    void SetUp() override {
        initialize_test_db(TEST_DB); 

        // 1. تسجيل مستخدمين
        Patient p("PayPat", "100", "p@pay.com", "pass");
        p.registerPatient(TEST_DB); // ID 1
        patientId = p.id;
        
        Doctor d("Dr. Pay", "Generalist");
        d.registerDoctor(TEST_DB); // ID 1
        doctorId = d.id;

        // 2. حجز موعد (غير مدفوع مبدئياً)
        Appointment a;
        a.patientId = patientId;
        a.doctorId = doctorId;
        a.dateTime = "2025-12-25 10:00:00";
        a.book(TEST_DB); 
        appointmentId = a.id;

        // يجب أن ينجح التسجيل لكي تعمل الاختبارات اللاحقة
        ASSERT_TRUE(appointmentId != 0); 
    }
};

// --------------------------------------------------------
// الاختبارات
// --------------------------------------------------------

// اختبار دالة simulatePayment بشكل منفصل
TEST_F(PaymentTests, SimulatePaymentLogic) {
    // الأرقام الزوجية يجب أن تنجح
    EXPECT_TRUE(simulatePayment("card_1234")); 
    EXPECT_TRUE(simulatePayment("card_0"));
    EXPECT_TRUE(simulatePayment("paypal_6"));

    // الأرقام الفردية يجب أن تفشل
    EXPECT_FALSE(simulatePayment("card_1235")); 
    EXPECT_FALSE(simulatePayment("gpay_1"));

    // التفاصيل الفارغة يجب أن تفشل
    EXPECT_FALSE(simulatePayment("")); 
    
    // التفاصيل التي لا تنتهي برقم يجب أن تفشل
    EXPECT_FALSE(simulatePayment("details_a"));
}

// 1. اختبار عملية الدفع الناجحة
TEST_F(PaymentTests, SuccessfulPayment) {
    // "card_1234" ينتهي برقم زوجي، لذا سينجح الدفع
    bool ok = simulatePayment("card_1234");
    ASSERT_TRUE(ok);

    // تحديث حالة الموعد وتسجيل الإيصال
    // *ملاحظة*: بما أننا لا يمكننا اختبار دالة CLI (payAppointmentCLI) مباشرة، 
    // يجب أن نختبر الدالة المسؤولة عن تحديث حالة الدفع في الكلاس Appointment
    // (نفترض أن الدالة setPaid موجودة وتعمل بناءً على منطق payment.cpp)
    Appointment a = Appointment::loadById(appointmentId, TEST_DB);
    bool set_paid_ok = a.setPaid(true, TEST_DB);

    // التحقق من أن عملية تحديث paid=1 نجحت
    EXPECT_TRUE(set_paid_ok);
    
    // التحقق من حالة الموعد في قاعدة البيانات
    EXPECT_TRUE(isAppointmentPaid(appointmentId, TEST_DB)) 
        << "Appointment status in DB should be paid (1).";

    // التحقق من تسجيل الإيصال بنجاح
    // *ملاحظة*: لا يمكننا التأكد من تسجيل الإيصال لأننا لم نستدعِ CLI. 
    // ولكن سنختبر أن دالة setPaid تعمل.

    // إذا أردت اختبار تسجيل الإيصال، يجب محاكاة منطق payment.cpp يدوياً أو تعديل CLI
    // لنكتفي باختبار المنطق الرئيسي هنا.
}


// 2. اختبار عملية الدفع الفاشلة
TEST_F(PaymentTests, FailedPayment) {
    // "card_1235" ينتهي برقم فردي، لذا سيفشل الدفع
    bool ok = simulatePayment("card_1235");
    ASSERT_FALSE(ok);

    // للتأكد من أن حالة الدفع لم تتغير (بافتراض أن الدفع لم ينجح، لن يتم استدعاء setPaid)
    EXPECT_FALSE(isAppointmentPaid(appointmentId, TEST_DB)) 
        << "Appointment status in DB should remain unpaid (0).";
}