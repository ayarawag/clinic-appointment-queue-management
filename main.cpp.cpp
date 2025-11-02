 #include <iostream>
#include <string>
using namespace std;

// feature1: book appointment
class Appointment {
private:
    int appointmentID;
    string date;
    string time;
    string status;

public:
    Appointment(int id = 0, string d = "", string t = "", string s = "Pending") {
        appointmentID = id;
        date = d;
        time = t;
        status = s;
    }

    bool bookAppointment(int patientID, int doctorID, string date, string time) {
        this->appointmentID = patientID + doctorID; // temporary unique ID logic
        this->date = date;
        this->time = time;
        this->status = "Booked";

        cout << "Appointment booked successfully!";
        cout << "Appointment ID: " << appointmentID << "Date: " << date << "Time: " << time << endl;
        return true;
    }

    bool cancelAppointment(int appointmentID) {
        if (this->appointmentID == appointmentID && status == "Booked") {
            status = "Cancelled";
            cout << "Appointment " << appointmentID << " has been cancelled";
            return true;
        }
        cout << "Appointment not found or already cancelled";
        return false;
    }

    bool rescheduleAppointment(int appointmentID, string newDate, string newTime) {
        if (this->appointmentID == appointmentID && status == "Booked") {
            date = newDate;
            time = newTime;
            cout << "Appointment rescheduled successfully!";
            cout << "New Date: " << newDate << "New Time: " << newTime << endl;
            return true;
        }
        cout << "Unable to reschedule appointment.";
        return false;
    }

    string getAppointmentDetails() {
        return "Appointment ID: " + to_string(appointmentID) + "Date: " + date + "Time: " + time + "Status: " + status;
    }
};

// feature2: electronic payment
// Payment Class (Parent)
class Payment {
protected:
    int paymentID;
    double amount;
    string method;

public:
    Payment(int id = 0, double amt = 0.0, string m = "") {
        paymentID = id;
        amount = amt;
        method = m;
    }

    virtual void processPayment() {
        cout << "Processing generic payment of $" << amount << " using " << method << endl;
    }

    virtual void refundPayment() {
        cout << "Refund processed for payment ID: " << paymentID << endl;
    }
};

// CashPayment (Subclass)

class CashPayment : public Payment {
private:
    string cashReceiptID;

public:
    CashPayment(int id, double amt, string receipt)
        : Payment(id, amt, "Cash"), cashReceiptID(receipt) {}

    void processPayment() override {
        cout << "Processing cash payment of $" << amount << ". Receipt ID: " << cashReceiptID << endl;
    }

    void refundPayment() override {
        cout << "Refunding cash payment. Receipt ID: " << cashReceiptID << endl;
    }
};

// CardPayment (Subclass)
class CardPayment : public Payment {
private:
    string cardNumber;

public:
    CardPayment(int id, double amt, string cardNum)
        : Payment(id, amt, "Card"), cardNumber(cardNum) {}

    void processPayment() override {
        cout << "Processing card payment of $" << amount << " using card: " << cardNumber << endl;
    }

    void refundPayment() override {
        cout << "Refunding card payment to card number: " << cardNumber << endl;
    }
};

// OnlinePayment (Subclass)
class OnlinePayment : public Payment {
private:
    string transactionID;

public:
    OnlinePayment(int id, double amt, string transID)
        : Payment(id, amt, "Online"), transactionID(transID) {}

    void processPayment() override {
        cout << "Processing online payment of $" << amount << ". Transaction ID: " << transactionID << endl;
    }

    void refundPayment() override {
        cout << "Refunding online payment. Transaction ID: " << transactionID << endl;
    }
      };

// Main Function
int main() {
    cout << "=== Appointment Feature ===\n";
    Appointment a1;
    a1.bookAppointment(101, 505, "2025-11-10", "09:00 AM");
    cout << a1.getAppointmentDetails() << endl;
    a1.rescheduleAppointment(606, "2025-11-12", "10:30 AM");
    a1.cancelAppointment(606);

    cout << " Payment Feature ";
    CashPayment cp(1, 50.0, "RCP123");
    cp.processPayment();

    CardPayment card(2, 100.0, "1234-5678-9876-1234");
    card.processPayment();

    OnlinePayment online(3, 75.0, "1234");
    online.processPayment();

     system ("pause");

    return 0;
}   