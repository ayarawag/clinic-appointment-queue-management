#include <gtest/gtest.h>
#include "../models/appointment.h"
#include "../database/db_connection.h"
#include <iostream>
#include <string>
#include <vector>

/**
 * ملف اختبارات مواعيد العيادة - الربط مع clinic.db الحقيقية
 * تغطي هذه الاختبارات ميزات الحجز، الإلغاء، الدفع، والبحث.
 */

using std::string;
using std::vector;

// المسار المطلق لقاعدة البيانات (تأكد من مطابقة المسار لجهازك)
const string REAL_DB = "/mnt/c/Users/ECS/Documents/GitHub/clinic-appointment-queue-management/clinic.db";

class AppointmentScenariosTests : public ::testing::Test {
protected:
    void SetUp() override {
        DBConnection::getInstance(REAL_DB);
    }
    void TearDown() override {
        DBConnection::destroyInstance();
    }
};

// ---------------------------------------------------------
// السيناريو 1: تحميل موعد موجود فعلياً (Data Integrity)
// الهدف: التأكد من أن النظام يقرأ الموعد رقم 1 بشكل صحيح.
// ---------------------------------------------------------
TEST_F(AppointmentScenariosTests, Scenario_LoadExistingAppointment) {
    std::cout << "[SCENARIO] تحميل بيانات الموعد رقم 1 من القاعدة..." << std::endl;
    
    Appointment app = Appointment::loadById(1, REAL_DB);
    
    ASSERT_NE(app.id, 0) << "فشل: الموعد رقم 1 غير موجود في القاعدة!";
    // بناءً على صورك، الموعد الأول يخص المريض رقم 1
    EXPECT_EQ(app.patientId, 1);
    std::cout << "[INFO] تم العثور على موعد المريض رقم: " << app.patientId << " بتاريخ: " << app.dateTime << std::endl;
}

// ---------------------------------------------------------
// السيناريو 2: تغيير موعد (Reschedule Feature)
// الهدف: التأكد من تحديث التاريخ والوقت في القاعدة.
// ---------------------------------------------------------
TEST_F(AppointmentScenariosTests, Scenario_RescheduleAppointment) {
    std::cout << "[SCENARIO] اختبار تغيير وقت الموعد رقم 1..." << std::endl;
    
    Appointment app = Appointment::loadById(1, REAL_DB);
    string originalDate = app.dateTime;
    string newDate = "2025-01-01 10:00:00";
    
    bool ok = app.reschedule(newDate, REAL_DB);
    EXPECT_TRUE(ok);
    
    // التحقق من الحفظ
    Appointment check = Appointment::loadById(1, REAL_DB);
    EXPECT_EQ(check.dateTime, newDate);
    
    // إرجاع التاريخ الأصلي للحفاظ على البيانات
    app.reschedule(originalDate, REAL_DB);
}

// ---------------------------------------------------------
// السيناريو 3: دفع قيمة الموعد (Payment Update)
// الهدف: التحقق من تحول حالة الدفع (Paid) من 0 إلى 1.
// ---------------------------------------------------------
TEST_F(AppointmentScenariosTests, Scenario_SetAppointmentPaid) {
    std::cout << "[SCENARIO] اختبار عملية دفع قيمة الموعد..." << std::endl;
    
    Appointment app = Appointment::loadById(1, REAL_DB);
    bool originalStatus = app.paid;
    
    bool ok = app.setPaid(true, REAL_DB);
    EXPECT_TRUE(ok);
    
    Appointment check = Appointment::loadById(1, REAL_DB);
    EXPECT_EQ(check.paid, 1);
    
    // إرجاع الحالة الأصلية
    app.setPaid(originalStatus, REAL_DB);
}

// ---------------------------------------------------------
// السيناريو 4: البحث المتقدم (Advanced Search Feature)
// الهدف: اختبار ميزة البحث عن مواعيد مريض معين في يوم محدد.
// ---------------------------------------------------------
TEST_F(AppointmentScenariosTests, Scenario_SearchAppointments) {
    std::cout << "[SCENARIO] البحث عن مواعيد المريض رقم 1..." << std::endl;
    
    // البحث عن مواعيد المريض 1 (بدون تحديد تاريخ أو طبيب لضمان ظهور نتائج)
    vector<Appointment> results = Appointment::search(1, "", 0, REAL_DB);
    
    EXPECT_FALSE(results.empty());
    std::cout << "[INFO] تم العثور على " << results.size() << " موعد/مواعيد للمريض." << std::endl;
    
    for(const auto& a : results) {
        EXPECT_EQ(a.patientId, 1);
    }
}

// ---------------------------------------------------------
// السيناريو 5: إلغاء موعد (Cancel Feature)
// الهدف: التأكد من تغير حالة الموعد إلى 'Cancelled'.
// ---------------------------------------------------------
TEST_F(AppointmentScenariosTests, Scenario_CancelAppointment) {
    std::cout << "[SCENARIO] اختبار إلغاء الموعد..." << std::endl;
    // سنقوم بحجز موعد مؤقت ثم إلغاؤه لتجنب تخريب المواعيد القديمة
    Appointment tempApp(1, 1, "2025-12-12 12:00:00");
    tempApp.book(REAL_DB);
    
    bool ok = tempApp.cancel(REAL_DB);
    EXPECT_TRUE(ok);
    EXPECT_EQ(tempApp.status, "Cancelled");
    
    // التحقق من القاعدة
    Appointment check = Appointment::loadById(tempApp.id, REAL_DB);
    EXPECT_EQ(check.status, "Cancelled");
}

// ---------------------------------------------------------
// السيناريو 6: حماية البيانات (Validation on Booking)
// الهدف: منع الحجز إذا كانت البيانات ناقصة (مثل عدم وجود تاريخ).
// ---------------------------------------------------------
TEST_F(AppointmentScenariosTests, Scenario_BlockInvalidBooking) {
    std::cout << "[SCENARIO] محاولة حجز موعد بدون تاريخ..." << std::endl;
    
    Appointment badApp(1, 1, ""); // تاريخ فارغ
    bool result = badApp.book(REAL_DB);
    
    EXPECT_FALSE(result);
    std::cout << "[INFO] تم منع الحجز بنجاح بسبب نقص البيانات." << std::endl;
}