#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <ctime>
#include <fstream>
#include <sstream>
#include <stdexcept>
using namespace std;

// Patient Class
class Patient {
public:
    int id;
    string name;
    string phone;
    string email;
    string passwordHash;
    int failedAttempts = 0;
    time_t lockUntil = 0;

    Patient(int id = 0, string n = "", string ph = "", string e = "", string p = "")
        : id(id), name(n), phone(ph), email(e), passwordHash(p) {}
};
string hashPassword(const string& password) {
    string hashed = "";
    for (char c : password)
        hashed += to_string((int)c + 7);
    return hashed;
}

bool isValidEmail(const string& email) {
    if (email.find('@') == string::npos || email.find('.') == string::npos)
        return false;

    int atPos = email.find('@');
    int dotPos = email.find('.', atPos);
    if (dotPos == string::npos)
        return false;

    if (atPos == 0 || dotPos == email.length() - 1)
        return false;

    return true;
}


bool isValidPassword(const string& password) {
    return password.length() >= 8;
}

//UserManager Class
class UserManager {
private:
    int nextID = 100;

public:
    int registerPatient() {
        try {
            string name, phone, email, password;
            cout << "\n--- User Registration (FR1) ---\n";
            cout << "Enter your name: ";
            getline(cin, name);
            cout << "Enter your phone: ";
            getline(cin, phone);
            cout << "Enter your email: ";
            getline(cin, email);
            cout << "Enter your password (min 8 chars): ";
            getline(cin, password);

            if (!isValidEmail(email))
                throw invalid_argument(" Invalid email");
            if (!isValidPassword(password))
                throw invalid_argument(" Password must be at least 8 characters");

            ifstream checkFile("patients.txt");
            if (!checkFile) {

                ofstream createFile("patients.txt");
                createFile.close();
                checkFile.open("patients.txt");
            }

            string line;
            int maxID = 99;
            while (getline(checkFile, line)) {
                stringstream ss(line);
                string idStr, n, ph, em, pass;
                getline(ss, idStr, ',');
                getline(ss, n, ',');
                getline(ss, ph, ',');
                getline(ss, em, ',');
                getline(ss, pass, ',');
                if (em == email || ph == phone)
                    throw runtime_error(" Email or phone already registered");
                if (!idStr.empty())
                    maxID = max(maxID, stoi(idStr));
            }
            nextID = maxID + 1;
            checkFile.close();

            string hash = hashPassword(password);
            ofstream file("patients.txt", ios::app);
            if (!file)
                throw runtime_error(" Unable to write to file");

            int newID = nextID++;
            file << newID << "," << name << "," << phone << "," << email << "," << hash << endl;
            file.close();

            cout << " Registration successful!\n";
            cout << "ur Patient ID: " << newID << endl;
            return newID;
        }
        catch (const exception& e) {
            cout << e.what() << endl;
            return -1;
        }
    }

    int loginPatient() {
        try {
            cout << "\nUser Login\n";
            string emailOrPhone, password;
            cout << "Enter email or phone: ";
            getline(cin, emailOrPhone);
            cout << "Enter password: ";
            getline(cin, password);

            ifstream file("patients.txt");
            if (!file)
                throw runtime_error(" No users registered");

            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string idStr, name, phone, email, hash;
                getline(ss, idStr, ',');
                getline(ss, name, ',');
                getline(ss, phone, ',');
                getline(ss, email, ',');
                getline(ss, hash, ',');

                if (email == emailOrPhone || phone == emailOrPhone) {
                    if (hash == hashPassword(password)) {
                        cout << "Login successful " << name << "\n";
                        return stoi(idStr);
                    }
                    else
                        throw invalid_argument(" Wrong password");
                }
            }
            throw invalid_argument(" No account found");
        }
        catch (const exception& e) {
            cout << e.what() << endl;
            return -1;
        }
    }
};

//Appointment Class
class Appointment {
public:
    int appointmentID;
    int patientID;
    int doctorID;
    string date;
    string time;
    string status;

    Appointment(int aid, int pid, int did, string d, string t)
        : appointmentID(aid), patientID(pid), doctorID(did), date(d), time(t), status("Booked") {}

    void setStatus(string newStatus) {
        status = newStatus;
    }
};

//Appointment Manager
class AppointmentManager {
private:
    vector<Appointment> appointments;

public:
    bool isSlotAvailable(int doctorID, string date, string time) {
        for (auto& appt : appointments) {
            if (appt.doctorID == doctorID && appt.date == date && appt.time == time)
                return false;
        }
        return true;
    }

    Appointment* bookAppointment(int patientID) {
        try {
            cout << "\n Book Appointment \n";
            int doctorID;
            string date, time;

            cout << "Enter Doctor ID (e.g. 501): ";
            cin >> doctorID;
            cin.ignore();
            cout << "Enter Date (YYYY-MM-DD): ";
            getline(cin, date);
            cout << "Enter Time (e.g. 09:30 AM): ";
            getline(cin, time);

            if (!isSlotAvailable(doctorID, date, time))
                throw runtime_error("already booked");

            int appointmentID = patientID * 1000 + doctorID;
            appointments.push_back(Appointment(appointmentID, patientID, doctorID, date, time));

            cout << "\n Appointment booked\n";
            cout << "Appointment ID: " << appointmentID
                << "\nDoctor ID: " << doctorID
                << "\nDate: " << date
                << "\nTime: " << time << endl;

            return &appointments.back();
        }
        catch (const exception& e) {
            cout << e.what() << endl;
            return nullptr;
        }
    }

    void showAllAppointments() {
        cout << "\nAll Appointments\n";
        if (appointments.empty()) {
            cout << "No appointments booked yet.\n";
            return;
        }
        for (auto& appt : appointments) {
            cout << "Appointment ID: " << appt.appointmentID
                << " | Patient ID: " << appt.patientID
                << " | Doctor ID: " << appt.doctorID
                << " | Date: " << appt.date
                << " | Time: " << appt.time
                << " | Status: " << appt.status << endl;
        }
    }
};

//Payment Classes
class Payment {
protected:
    int paymentID;
    double amount;
    string method;

public:
    Payment(int id, double amt, string m) : paymentID(id), amount(amt), method(m) {}
    virtual void processPayment(Appointment& appt) = 0;
    virtual void refundPayment() = 0;
    virtual ~Payment() {}
};

class ElectronicPayment : public Payment {
protected:
    string transactionID;

public:
    ElectronicPayment(int id, double amt, string m, string trans)
        : Payment(id, amt, m), transactionID(trans) {}

    void sendReceipt() {
        cout << " sent successfully\n";
    }

    void showError() {
        cout << "Payment failed \n";
    }
};

class CardPayment : public ElectronicPayment {
private:
    string cardNumber;
    string cardType;

public:
    CardPayment(int id, double amt, string type, string cardNum, string trans)
        : ElectronicPayment(id, amt, type, trans), cardNumber(cardNum), cardType(type) {}

    void processPayment(Appointment& appt) override {
        try {
            cout << "\n Processing " << cardType << " card payment of $" << amount << "\n";
            if (amount <= 0)
                throw invalid_argument(" Invalid payment");
            appt.setStatus("Paid");
            cout << " Appointment marked as 'Paid'.\n";
            sendReceipt();
        }
        catch (const exception& e) {
            showError();
            cout << e.what() << endl;
        }
    }

    void refundPayment() override {
        cout << "Refunding card payment to " << cardType << " card: " << cardNumber << endl;
    }
};

class DigitalWalletPayment : public ElectronicPayment {
private:
    string walletType;

public:
    DigitalWalletPayment(int id, double amt, string wallet, string trans)
        : ElectronicPayment(id, amt, "Digital Wallet", trans), walletType(wallet) {}

    void processPayment(Appointment& appt) override {
        try {
            cout << "\n Processing " << walletType << " payment of $" << amount << "\n";
            if (amount <= 0)
                throw invalid_argument("Invalid payment");
            appt.setStatus("Paid");
            cout << " Appointment marked as 'Paid'.\n";
            sendReceipt();
        }
        catch (const exception& e) {
            showError();
            cout << e.what() << endl;
        }
    }

    void refundPayment() override {
        cout << "Refunding payment via " << walletType << "...\n";
    }
};

//Main
int main() {
    cout << "=== Clinic Appointment System ===\n";
    UserManager userManager;
    AppointmentManager appointmentManager;

    while (true) {
        try {
            cout << "\n1. Register (New Patient)\n";
            cout << "2. Login (Existing Patient)\n";
            cout << "3. Exit\n";
            cout << "Enter your choice: ";
            int choice;
            cin >> choice;
            cin.ignore();

            if (choice == 1) {
                userManager.registerPatient();
            }
            else if (choice == 2) {
                int patientID = userManager.loginPatient();
                if (patientID != -1) {
                    int option;
                    do {
                        cout << "\n--- Patient Menu ---\n";
                        cout << "1. Book Appointment\n";
                        cout << "2. View Appointments\n";
                        cout << "3. Logout\n";
                        cout << "Enter your choice: ";
                        cin >> option;
                        cin.ignore();

                        if (option == 1) {
                            Appointment* appt = appointmentManager.bookAppointment(patientID);
                            if (appt != nullptr) {
                                cout << "\nSelect Payment Method:\n";
                                cout << "1. Card Payment (Visa/MasterCard)\n";
                                cout << "2. Digital Wallet (PayPal/Google Pay)\n";
                                cout << "Enter choice: ";
                                int payChoice;
                                cin >> payChoice;
                                cin.ignore();

                                if (payChoice == 1) {
                                    CardPayment card(1, 100.0, "Visa", "1234-5678-9999", "TX123");
                                    card.processPayment(*appt);
                                }
                                else if (payChoice == 2) {
                                    DigitalWalletPayment wallet(2, 100.0, "PayPal", "TX789");
                                    wallet.processPayment(*appt);
                                }
                            }
                        }
                        else if (option == 2)
                            appointmentManager.showAllAppointments();

                    } while (option != 3);
                }
            }
            else if (choice == 3) {
                cout << "bye\n";
                break;
            }
            else {
                throw invalid_argument(" Invalid menu choice");
            }
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
    return 0;
}
