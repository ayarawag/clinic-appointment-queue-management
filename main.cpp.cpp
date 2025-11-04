#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <ctime>
#include <fstream>
#include <sstream>
using namespace std;

// ---------------- Patient Class ----------------
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

// ---------------- Helper Functions ----------------
string hashPassword(const string& password) {
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

// ---------------- UserManager Class ----------------
class UserManager {
private:
    int nextID = 100;

public:
    // Registration (FR1)
    int registerPatient() {
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

        if (!isValidEmail(email)) {
            cout << " Invalid email format.\n";
            return -1;
        }
        if (!isValidPassword(password)) {
            cout << " Password must be at least 8 characters.\n";
            return -1;
        }

        // check duplicates in file
        ifstream checkFile("patients.txt");
        string line;
        while (getline(checkFile, line)) {
            stringstream ss(line);
            string idStr, n, ph, em, pass;
            getline(ss, idStr, ',');
            getline(ss, n, ',');
            getline(ss, ph, ',');
            getline(ss, em, ',');
            getline(ss, pass, ',');
            if (em == email || ph == phone) {
                cout << " Email or phone already registered.\n";
                return -1;
            }
        }
        checkFile.close();

        string hash = hashPassword(password);
        ofstream file("patients.txt", ios::app);
        int newID = nextID++;
        file << newID << "," << name << "," << phone << "," << email << "," << hash << endl;
        file.close();

        cout << " Registration successful!\n";
        cout << "Your Patient ID: " << newID << endl;
        return newID;
    }

    // Login (FR2)
    int loginPatient() {
        cout << "\n--- User Login (FR2) ---\n";
        string emailOrPhone, password;
        cout << "Enter email or phone: ";
        getline(cin, emailOrPhone);
        cout << "Enter password: ";
        getline(cin, password);

        ifstream file("patients.txt");
        if (!file) {
            cout << " No users registered yet.\n";
            return -1;
        }

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
                    cout << "Login successful. Welcome, " << name << "!\n";
                    return stoi(idStr);
                } else {
                    cout << " Wrong password.\n";
                    return -1;
                }
            }
        }

        cout << " No account found with that email or phone.\n";
        return -1;
    }
};

// ---------------- Appointment Class ----------------
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
};

// ---------------- Appointment Manager ----------------
class AppointmentManager {
private:
    vector<Appointment> appointments;

public:
    bool isSlotAvailable(int doctorID, string date, string time) {
        for (auto &appt : appointments) {
            if (appt.doctorID == doctorID && appt.date == date && appt.time == time) {
                return false; // slot already booked
            }
        }
        return true;
    }

    void bookAppointment(int patientID) {
        cout << "\n--- Book Appointment ---\n";
        int doctorID;
        string date, time;

        cout << "Enter Doctor ID (e.g. 501): ";
        cin >> doctorID;
        cin.ignore();
        cout << "Enter Date (YYYY-MM-DD): ";
        getline(cin, date);
        cout << "Enter Time (e.g. 09:30 AM): ";
        getline(cin, time);

        if (!isSlotAvailable(doctorID, date, time)) {
            cout << " This time slot is already booked. Please choose another.\n";
            return;
        }

        int appointmentID = patientID * 1000 + doctorID;
        appointments.push_back(Appointment(appointmentID, patientID, doctorID, date, time));

        cout << "\n Appointment booked successfully!\n";
        cout << "Appointment ID: " << appointmentID << "\nDoctor ID: " << doctorID
             << "\nDate: " << date << "\nTime: " << time << endl;
    }

    void showAllAppointments() {
        cout << "\n--- All Appointments ---\n";
        if (appointments.empty()) {
            cout << "No appointments booked yet.\n";
            return;
        }
        for (auto &appt : appointments) {
            cout << "Appointment ID: " << appt.appointmentID
                 << " | Patient ID: " << appt.patientID
                 << " | Doctor ID: " << appt.doctorID
                 << " | Date: " << appt.date
                 << " | Time: " << appt.time
                 << " | Status: " << appt.status << endl;
        }
    }
};

// ---------------- Main ----------------
int main() {
    cout << "=== Clinic Appointment System ===\n";
    UserManager userManager;
    AppointmentManager appointmentManager;

    while (true) {
        cout << "\n1. Register (New Patient)\n";
        cout << "2. Login (Existing Patient)\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            userManager.registerPatient();
        } else if (choice == 2) {
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

                    if (option == 1)
                        appointmentManager.bookAppointment(patientID);
                    else if (option == 2)
                        appointmentManager.showAllAppointments();

                } while (option != 3);
            }
        } else if (choice == 3) {
            cout << "Goodbye!\n";
            break;
        } else {
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}
