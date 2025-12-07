#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <iostream>

class Patient {
private:
    int id;
    std::string name;
    std::string phone;
    std::string email;
    std::string password;

public:
    // المنشئات (Constructors)
    Patient();
    Patient(int id, const std::string& name, const std::string& phone, const std::string& email, const std::string& password);

    // Getters
    int getId() const;
    std::string getName() const;
    std::string getPhone() const;
    std::string getEmail() const;
    std::string getPassword() const;

    // Setters
    void setName(const std::string& name);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);
    void setPassword(const std::string& password);

    // وظائف قاعدة البيانات
    bool registerPatient(const std::string& name, const std::string& phone, const std::string& email, const std::string& password);
    // دالة جديدة لتسجيل الدخول في قاعدة البيانات
    bool login(const std::string& email, const std::string& password); 

};

#endif // PATIENT_H