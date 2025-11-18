#include <iostream>
#include "database/db_connection.h"
#include "models/patient.h"
#include "models/doctor.h"
#include "models/appointment.h"
#include "models/queue.h"

void registerUser();
bool loginUser();
void bookAppointment();
void payAppointment();
void updateQueue();
void cancelAppointment();
void setDoctorSchedule();

int main() {
    std::cout << "Program started...\n";

    DBConnection db("clinic.db");

    int choice;
    do {
        std::cout << "\n1- Register\n2- Login\n3- Book Appointment\n4- Pay\n5- Update Queue\n6- Cancel Appointment\n7- Set Doctor Schedule\n0- Exit\nChoice: ";
        std::cin >> choice;

        switch(choice) {
            case 1: registerUser(); break;
            case 2: loginUser(); break;
            case 3: bookAppointment(); break;
            case 4: payAppointment(); break;
            case 5: updateQueue(); break;
            case 6: cancelAppointment(); break;
            case 7: setDoctorSchedule(); break;
        }
    } while(choice != 0);

    return 0;
}