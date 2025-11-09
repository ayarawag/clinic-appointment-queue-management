#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <sqlite3.h>

using namespace std;

// ----------------- Utilities -----------------
string hashPassword(const string& password) {
    string hashed = "";
    for (char c : password) hashed += to_string((int)c + 7);
    return hashed;
}

bool isValidEmail(const string& email) {
    size_t atPos = email.find('@');
    size_t dotPos = email.find('.', atPos);
    return (atPos != string::npos && dotPos != string::npos && atPos > 0 && dotPos < email.length() - 1);
}

bool isValidPassword(const string& password) {
    return password.length() >= 8;
}

long long nowEpoch() { return static_cast<long long>(time(nullptr)); }

// ----------------- UserManager -----------------
class UserManager {
    sqlite3* db;
public:
    UserManager(sqlite3* _db) : db(_db) {}

    bool initTables() {
        const char* sqlPatients = R"(
            CREATE TABLE IF NOT EXISTS patients (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                phone TEXT UNIQUE NOT NULL,
                email TEXT UNIQUE NOT NULL,
                password TEXT NOT NULL
            );
        )";
        char* err = nullptr;
        if (sqlite3_exec(db, sqlPatients, nullptr, nullptr, &err) != SQLITE_OK) {
            cerr << "Error creating patients table: " << (err ? err : "") << endl;
            sqlite3_free(err);
            return false;
        }
        return true;
    }

    int registerPatient() {
        string name, phone, email, password;
        cout << "\n--- User Registration ---\n";
        cin.ignore();
        cout << "Enter your name: "; getline(cin, name);
        cout << "Enter your phone: "; getline(cin, phone);
        cout << "Enter your email: "; getline(cin, email);
        cout << "Enter your password (min 8 chars): "; getline(cin, password);

        if (!isValidEmail(email)) { cout << "Invalid email format.\n"; return -1; }
        if (!isValidPassword(password)) { cout << "Password must be at least 8 characters.\n"; return -1; }

        const char* sql = "INSERT INTO patients (name, phone, email, password) VALUES (?, ?, ?, ?);";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cout << "DB prepare error: " << sqlite3_errmsg(db) << endl;
            return -1;
        }

        sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, phone.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, email.c_str(), -1, SQLITE_TRANSIENT);
        string hashed = hashPassword(password);
        sqlite3_bind_text(stmt, 4, hashed.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            cout << "Error inserting user: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return -1;
        }

        sqlite3_finalize(stmt);
        int newID = static_cast<int>(sqlite3_last_insert_rowid(db));
        cout << "Registration successful! Your Patient ID: " << newID << endl;
        return newID;
    }

    int loginPatient() {
        string emailOrPhone, password;
        cin.ignore();
        cout << "\n--- User Login ---\n";
        cout << "Enter email or phone: "; getline(cin, emailOrPhone);
        cout << "Enter password: "; getline(cin, password);

        const char* sql = "SELECT id, name, password FROM patients WHERE email = ? OR phone = ?;";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cout << "DB prepare error: " << sqlite3_errmsg(db) << endl;
            return -1;
        }

        sqlite3_bind_text(stmt, 1, emailOrPhone.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, emailOrPhone.c_str(), -1, SQLITE_TRANSIENT);

        int patientID = -1;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            string dbPassStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            if (dbPassStr == hashPassword(password)) {
                patientID = sqlite3_column_int(stmt, 0);
                cout << "Login successful! Welcome " << reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)) << endl;
            } else cout << "Wrong password.\n";
        } else cout << "No account found.\n";

        sqlite3_finalize(stmt);
        return patientID;
    }
};

// ----------------- AppointmentManager -----------------
class AppointmentManager {
    sqlite3* db;
public:
    AppointmentManager(sqlite3* _db) : db(_db) {}

    bool initTables() {
        const char* sqlAppointments = R"(
            CREATE TABLE IF NOT EXISTS appointments (
                appointmentID INTEGER PRIMARY KEY AUTOINCREMENT,
                patientID INTEGER NOT NULL,
                doctorID INTEGER NOT NULL,
                date TEXT NOT NULL,
                time TEXT NOT NULL,
                status TEXT NOT NULL,
                bookingTime INTEGER NOT NULL,
                emergency INTEGER DEFAULT 0,
                queue_position INTEGER,
                FOREIGN KEY(patientID) REFERENCES patients(id)
            );
        )";
        char* err = nullptr;
        if (sqlite3_exec(db, sqlAppointments, nullptr, nullptr, &err) != SQLITE_OK) {
            cerr << "Error creating appointments table: " << (err ? err : "") << endl;
            sqlite3_free(err);
            return false;
        }
        return true;
    }

    bool isSlotAvailable(int doctorID, const string& date, const string& timeStr) {
        const char* sql = "SELECT COUNT(*) FROM appointments WHERE doctorID=? AND date=? AND time=? AND status!='Cancelled';";
        sqlite3_stmt* stmt = nullptr;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_int(stmt, 1, doctorID);
        sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, timeStr.c_str(), -1, SQLITE_TRANSIENT);
        bool available = true;
        if (sqlite3_step(stmt) == SQLITE_ROW) available = (sqlite3_column_int(stmt, 0) == 0);
        sqlite3_finalize(stmt);
        return available;
    }

    int nextQueuePosition(int doctorID) {
        const char* sql = "SELECT COALESCE(MAX(queue_position),0)+1 FROM appointments WHERE doctorID=? AND status!='Cancelled';";
        sqlite3_stmt* stmt = nullptr;
        int pos = 1;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, doctorID);
            if (sqlite3_step(stmt) == SQLITE_ROW) pos = sqlite3_column_int(stmt,0);
            sqlite3_finalize(stmt);
        }
        return pos;
    }

    // ---------------- Queue Management ----------------
    void reorderQueue(int doctorID) {
        const char* sql = "SELECT appointmentID FROM appointments WHERE doctorID=? AND status!='Cancelled' ORDER BY emergency DESC, bookingTime ASC;";
        sqlite3_stmt* stmt = nullptr;
        if(sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr)!=SQLITE_OK) return;
        sqlite3_bind_int(stmt,1,doctorID);
        vector<int> ordered;
        while(sqlite3_step(stmt)==SQLITE_ROW) ordered.push_back(sqlite3_column_int(stmt,0));
        sqlite3_finalize(stmt);

        const char* upd = "UPDATE appointments SET queue_position=? WHERE appointmentID=?;";
        sqlite3_stmt* ustmt = nullptr;
        if(sqlite3_prepare_v2(db,upd,-1,&ustmt,nullptr)!=SQLITE_OK) return;
        for(size_t i=0;i<ordered.size();++i){
            sqlite3_reset(ustmt);
            sqlite3_bind_int(ustmt,1,i+1);
            sqlite3_bind_int(ustmt,2,ordered[i]);
            sqlite3_step(ustmt);
        }
        sqlite3_finalize(ustmt);
    }

    void showQueue(int doctorID) {
        const char* sql = "SELECT appointmentID, patientID, emergency, queue_position FROM appointments WHERE doctorID=? AND status!='Cancelled' ORDER BY queue_position ASC;";
        sqlite3_stmt* stmt=nullptr;
        if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return;
        sqlite3_bind_int(stmt,1,doctorID);
        cout<<"\n--- Queue for Doctor "<<doctorID<<" ---\n";
        while(sqlite3_step(stmt)==SQLITE_ROW){
            int apptID=sqlite3_column_int(stmt,0);
            int pid=sqlite3_column_int(stmt,1);
            int em=sqlite3_column_int(stmt,2);
            int qpos=sqlite3_column_int(stmt,3);
            cout<<"Pos "<<qpos<<": ApptID="<<apptID<<" PatientID="<<pid<<" Emergency="<<(em?"Yes":"No")<<endl;
        }
        sqlite3_finalize(stmt);
    }

    vector<int> getCurrentQueue(int doctorID) {
        const char* sql = "SELECT appointmentID FROM appointments WHERE doctorID=? AND status!='Cancelled';";
        sqlite3_stmt* stmt=nullptr;
        vector<int> result;
        if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return result;
        sqlite3_bind_int(stmt,1,doctorID);
        while(sqlite3_step(stmt)==SQLITE_ROW) result.push_back(sqlite3_column_int(stmt,0));
        sqlite3_finalize(stmt);
        return result;
    }

    void manualReorderQueue(int doctorID) {
        showQueue(doctorID);
        cout<<"Enter appointment IDs in desired order (end with 0): ";
        vector<int> newOrder; int id;
        while(cin>>id && id!=0) newOrder.push_back(id); cin.ignore();

        vector<int> current = getCurrentQueue(doctorID);
        sort(current.begin(), current.end());
        vector<int> temp = newOrder; sort(temp.begin(), temp.end());
        if(current!=temp){ cout<<"Order invalid, abort.\n"; return; }

        const char* upd = "UPDATE appointments SET queue_position=? WHERE appointmentID=?;";
        sqlite3_stmt* stmt=nullptr;
        if(sqlite3_prepare_v2(db,upd,-1,&stmt,nullptr)!=SQLITE_OK) return;
        for(size_t i=0;i<newOrder.size();++i){
            sqlite3_reset(stmt);
            sqlite3_bind_int(stmt,1,i+1);
            sqlite3_bind_int(stmt,2,newOrder[i]);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
        cout<<"Queue reordered successfully.\n";
        showQueue(doctorID);
    }

    // ---------------- Book Appointment ----------------
    int bookAppointment(int patientID) {
        int doctorID;
        string date,timeStr;
        cout<<"\n--- Book Appointment ---\n";
        cout<<"Enter Doctor ID: "; cin>>doctorID; cin.ignore();
        cout<<"Enter Date (YYYY-MM-DD): "; getline(cin,date);
        cout<<"Enter Time (HH:MM AM/PM): "; getline(cin,timeStr);
        cout<<"Emergency? (y/n): "; string em; getline(cin,em);
        bool isEmergency=(em=="y"||em=="Y");

        if(!isEmergency && !isSlotAvailable(doctorID,date,timeStr)){
            cout<<"Slot already booked.\n"; return -1;
        }

        long long btime=nowEpoch();
        int queuePos=nextQueuePosition(doctorID);

        const char* sql="INSERT INTO appointments(patientID,doctorID,date,time,status,bookingTime,emergency,queue_position) VALUES(?,?,?,?, 'Booked',?,?,?,?);";
        sqlite3_stmt* stmt=nullptr;
        if(sqlite3_prepare_v2(db,sql,-1,&stmt,nullptr)!=SQLITE_OK) return -1;
        sqlite3_bind_int(stmt,1,patientID); sqlite3_bind_int(stmt,2,doctorID);
        sqlite3_bind_text(stmt,3,date.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt,4,timeStr.c_str(),-1,SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt,5,btime); sqlite3_bind_int(stmt,6,isEmergency?1:0);
        sqlite3_bind_int(stmt,7,queuePos);

        if(sqlite3_step(stmt)!=SQLITE_DONE){ sqlite3_finalize(stmt); return -1; }
        sqlite3_finalize(stmt);
        int apptID=static_cast<int>(sqlite3_last_insert_rowid(db));

        if(isEmergency) reorderQueue(doctorID); // ترتيب تلقائي للطوارئ

        cout<<"Appointment booked! ID: "<<apptID<<endl;
        cout<<"Select Payment: 1.Card 2.Wallet: "; int pay; cin>>pay; cin.ignore();
        if(pay==1||pay==2){
            const char* paySql="UPDATE appointments SET status='Paid' WHERE appointmentID=?;";
            sqlite3_stmt* pstmt=nullptr; sqlite3_prepare_v2(db,paySql,-1,&pstmt,nullptr);
            sqlite3_bind_int(pstmt,1,apptID); sqlite3_step(pstmt); sqlite3_finalize(pstmt);
            cout<<"Payment successful!\n";
        }

        return apptID;
    }
};

// ----------------- Main -----------------
int main(){
    cout<<"=== Clinic Appointment System ===\n";
    sqlite3* db=nullptr;
    if(sqlite3_open("clinic.db",&db)!=SQLITE_OK){ cerr<<"Failed to open DB\n"; return 1; }

    UserManager userManager(db);
    AppointmentManager appointmentManager(db);
    if(!userManager.initTables()||!appointmentManager.initTables()){ cerr<<"Failed to init DB tables\n"; return 1; }

    while(true){
        cout<<"\n1.Register\n2.Login\n3.Doctor Queue Tools\n4.Exit\nChoice: ";
        int choice; if(!(cin>>choice)){cin.clear(); string tmp; getline(cin,tmp); continue;}
        if(choice==1) userManager.registerPatient();
        else if(choice==2){
            int pid=userManager.loginPatient();
            if(pid!=-1) appointmentManager.bookAppointment(pid);
        }
        else if(choice==3){
            cout<<"Enter Doctor ID for Queue Management: "; int did; cin>>did; cin.ignore();
            appointmentManager.showQueue(did);
            cout<<"Do you want to reorder manually? (y/n): "; string r; getline(cin,r);
            if(r=="y"||r=="Y") appointmentManager.manualReorderQueue(did);
        }
        else if(choice==4) break;
        else cout<<"Invalid choice.\n";
    }

    sqlite3_close(db);
    return 0;
}