Clinic Appointment & Queue Management System  

 Project Overview  
This project is part of the Course Project – First Sprint.  
It is a Clinic Appointment & Queue Management System designed to help patients book appointments, manage payments, and receive notifications easily.  

The system allows:  
- Patients to register and log in.  
- Booking, canceling, and rescheduling appointments.  
- Doctors to manage their schedules.  
- Admins to oversee the system.  
- Multiple payment methods (cash, card, online).  



 Features Implemented  
1. User Registration  
   - Allows new patients to create an account by entering their name, phone number, email, and password.  
   - The system validates the email format and ensures the password is at least 8 characters long.  

2. User Login  
   - Enables registered patients to log in using their email and password.  
   - The system verifies credentials through the database and grants access upon success.  

3. Book Appointment Feature  
   - Patients can book new appointments with doctors.  
   - The system verifies date and time availability before confirming the booking.  

4. Electronic Payment Feature  
   - Allows patients to pay using cash, card, or online payment.  
   - Each payment type is implemented as a subclass of the Payment class.  


 Technologies Used  
- Programming Language: C++  
- UML Design Tool: Draw.io  
- Version Control: GitHub Desktop  
- Development Environment: Visual Studio Code  


#UML Design  
- Class Diagram:  
  Represents the main system classes (User, Patient, Doctor, Appointment, Payment, Notification, etc.) and their relationships.  

- Sequence Diagrams:  
  - User Registration  
  - Login  
  - Book Appointment  
  - Electronic Payment  


 How to Run the Project  
1. Clone the repository:  
   bash
   git clone https://github.com/ayarawag/clinic-appointment-queue-management.git