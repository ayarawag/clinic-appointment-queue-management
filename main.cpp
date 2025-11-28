#include <iostream>
using namespace std;

void registerUser();
bool loginUser();
void bookAppointment();
void cancelAppointment();
void rescheduleAppointment();
void updateQueue();
void reorderQueue();
void payAppointment();
void setDoctorSchedule();
void viewAppointments();
void runReminders(int);

int main() {
    using namespace std;
    int choice;
    do {
        cout << "\n--- Clinic System ---\n";
        cout << "1) Register\n2) Login\n3) Book\n4) Cancel\n5) Reschedule\n6) Add to Queue\n7) Reorder Queue\n8) Pay\n9) Doctor Schedule\n10) View Appointments\n11) Run Reminders\n0) Exit\nChoice: ";
        cin >> choice;
        switch(choice) {
            case 1: registerUser(); break;
            case 2: loginUser(); break;
            case 3: bookAppointment(); break;
            case 4: cancelAppointment(); break;
            case 5: rescheduleAppointment(); break;
            case 6: updateQueue(); break;
            case 7: reorderQueue(); break;
            case 8: payAppointment(); break;
            case 9: setDoctorSchedule(); break;
            case 10: viewAppointments(); break;
            case 11: {
                int m; cout << "Minutes before: "; cin >> m;
                runReminders(m);
                break;
            }
        }
    } while(choice != 0);
    return 0;
}