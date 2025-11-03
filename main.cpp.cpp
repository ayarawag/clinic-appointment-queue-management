#include <iostream>
#include <string>
#include <vector>
#include <regex>
using namespace std;

// ---------------- Patient Class ----------------
class Patient {
public:
    int id;
    string name;
    string phone;
    string email;
    string passwordHash;

    Patient(int id = 0, string n = "", string ph = "", string e = "", string p = "")
        : id(id), name(n), phone(ph), email(e), passwordHash(p) {}
};

// ---------------- Helper Functions ----------------
string hashPassword(const string& password) {
    // Simple password hashing simulation (for demo purposes)
    string hashed = "";
    for (char c : password)
        hashed += to_string((int)c + 7);
    return hashed;
}

bool isValidEmail(const string& email) {
    const regex pattern(R"((\w+)(\.|_)?(\w*)@(\w+)\.(\w+))");
    return regex_match(email, pattern);
}

bool isValidPassword(const string& password) {
    return password.length() >= 8;
}

// ---------------- UserManager Class ( Registration) ----------------
class UserManager {
private:
    vector<Patient> patients;
    int nextID = 100;

public:
    int registerPatient(const string& name, const string& phone, const string& email, const string& password) {
        if (!isValidEmail(email)) {
            cout << " Invalid email format.\n";
            return -1;
        }
        if (!isValidPassword(password)) {
            cout << " Password must be at least 8 characters.\n";
            return -1;
        }

        // Check duplicates
        for (auto &p : patients) {
            if (p.email == email || p.phone == phone) {
                cout << " Email or phone already registered.\n";
                return -1;
            }
        }

        string hash = hashPassword(password);
        Patient newPatient(nextID++, name, phone, email, hash);
        patients.push_back(newPatient);

        cout << "Registration successful!\n";
        cout << "Your Patient ID: " << newPatient.id << endl;
        return newPatient.id;
    }

    void showAllPatients() {
        cout << "\n--- Registered Patients ---\n";
        for (auto &p : patients) {
            cout << "ID: " << p.id << " | Name: " << p.name
                 << " | Email: " << p.email
                 << " | Phone: " << p.phone << endl;
        }
    }
};

// ---------------- Appointment Class ----------------
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
        this->appointmentID = patientID * 1000 + doctorID;
        this->date = date;
        this->time = time;
        this->status = "Booked";

        cout << "\n Appointment booked successfully!\n";
        cout << "Appointment ID: " << appointmentID
             << "\nDate: " << date
             << "\nTime: " << time << endl;
        return true;
    }

    string getAppointmentDetails() {
        return "Appointment ID: " + to_string(appointmentID) +
               " | Date: " + date + " | Time: " + time + " | Status: " + status;
    }
};

// ---------------- Payment Classes ----------------
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
        cout << "Processing payment of $" << amount << " via " << method << endl;
    }

    virtual void refundPayment() {
        cout << "Refund processed for payment ID: " << paymentID << endl;
    }
};

class CashPayment : public Payment {
private:
    string cashReceiptID;

public:
    CashPayment(int id, double amt, string receipt)
        : Payment(id, amt, "Cash"), cashReceiptID(receipt) {}

    void processPayment() override {
        cout << "Processing cash payment of $" << amount<< ". Receipt ID: " << cashReceiptID << endl;
    }

    void refundPayment() override {
        cout << "Refunding cash payment. Receipt ID: " << cashReceiptID << endl;
    }
};

class CardPayment : public Payment {
private:
    string cardNumber;

public:
    CardPayment(int id, double amt, string cardNum)
        : Payment(id, amt, "Card"), cardNumber(cardNum) {}

    void processPayment() override {
        cout << "Processing card payment of $" << amount
             << " using card: " << cardNumber << endl;
    }

    void refundPayment() override {
        cout << "Refunding card payment to card number: " << cardNumber << endl;
    }
};

class OnlinePayment : public Payment {
private:
    string transactionID;

public:
    OnlinePayment(int id, double amt, string transID)
        : Payment(id, amt, "Online"), transactionID(transID) {}

    void processPayment() override {
        cout << "Processing online payment of $" << amount
             << ". Transaction ID: " << transactionID << endl;
    }

    void refundPayment() override {
        cout << "Refunding online payment. Transaction ID: " << transactionID << endl;
    }
};

// ---------------- Main ----------------
int main() {
    cout << "=== Clinic Appointment System ===\n\n";

    UserManager userManager;

    cout << "--- User Registration ---\n";
    string name, phone, email, password;

    cout << "Enter your name: ";
    getline(cin, name);
    cout << "Enter your phone: ";
    getline(cin, phone);
    cout << "Enter your email: ";
    getline(cin, email);
    cout << "Enter your password (min 8 chars): ";
    getline(cin, password);

    int patientID = userManager.registerPatient(name, phone, email, password);

    if (patientID != -1) {
        cout << "\n Registration completed successfully!\n";
        userManager.showAllPatients();

        // Optional: show a booking example to demonstrate the system’s structure
        cout << "\n--- Appointment Demo ---\n";
        Appointment appt;
        appt.bookAppointment(patientID, 505, "2025-11-20", "09:00 AM");
        cout << appt.getAppointmentDetails() << endl;

        cout << "\n--- Payment Demo ---\n";
        CashPayment cash(1, 50.0, "RCP123");
        cash.processPayment();
    } else {
        cout << "\n Registration failed. Please try again.\n";
    }

    cout << "\n=== End of Program ===\n";
    return 0;
}
