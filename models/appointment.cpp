#include "appointment.h"
#include "../database/db_connection.h"
#include <sstream>
#include <string>
#include <cstdlib>
using std::stoi;

Appointment::Appointment(){ id=0; paid=0; status="Scheduled"; }

Appointment Appointment::loadById(int aid,const std::string& db){
    Appointment a;
    DBConnection x(db);
    x.query("SELECT id,patient_id,doctor_id,date_time,status,paid FROM appointments WHERE id="+std::to_string(aid),
        [](void* u,int,char** v,char**){
            Appointment* a=(Appointment*)u;
            a->id=stoi(v[0]); a->patientId=stoi(v[1]); a->doctorId=stoi(v[2]);
            a->dateTime=v[3]; a->status=v[4]; a->paid=stoi(v[5]);
            return 0; }, &a);
    return a;
}

bool Appointment::book(const std::string& db){
    DBConnection x(db);
    bool exists=false;
    x.query("SELECT id FROM appointments WHERE doctor_id="+std::to_string(doctorId)+
            " AND date_time='"+dateTime+"' LIMIT 1;",
        [](void* u,int,char** v,char**){ if(v[0]) *(bool*)u=true; return 0; }, &exists);

    if(exists) return false;

    std::ostringstream q;
    q<<"INSERT INTO appointments(patient_id,doctor_id,date_time,status,paid) VALUES("
      <<patientId<<","<<doctorId<<",'"
      <<dateTime<<"','Scheduled',0);";
    return x.execute(q.str());
}

bool Appointment::cancel(const std::string& db){
    DBConnection x(db);
    return x.execute("UPDATE appointments SET status='Cancelled' WHERE id="+std::to_string(id));
}

bool Appointment::reschedule(std::string newDate,const std::string& db){
    DBConnection x(db);
    std::ostringstream q;
    q<<"UPDATE appointments SET date_time='"<<newDate<<"', status='Rescheduled' WHERE id="<<id;
    return x.execute(q.str());
}

bool Appointment::setPaid(const std::string& db){
    DBConnection x(db);
    return x.execute("UPDATE appointments SET paid=1 WHERE id="+std::to_string(id));
}

std::vector<Appointment> Appointment::getByPatient(int pid,const std::string& db){
    std::vector<Appointment> list;
    DBConnection x(db);
    x.query("SELECT id,patient_id,doctor_id,date_time,status,paid FROM appointments WHERE patient_id="+std::to_string(pid),
        [](void* u,int,char** v,char**){
            auto* l=(std::vector<Appointment>*)u;
            Appointment a;
            a.id=stoi(v[0]); a.patientId=stoi(v[1]); a.doctorId=stoi(v[2]);
            a.dateTime=v[3]; a.status=v[4]; a.paid=stoi(v[5]);
            l->push_back(a); return 0; }, &list);
    return list;
}

std::vector<Appointment> Appointment::getByDoctor(int did,const std::string& db){
    std::vector<Appointment> list;
    DBConnection x(db);
    x.query("SELECT id,patient_id,doctor_id,date_time,status,paid FROM appointments WHERE doctor_id="+std::to_string(did),
        [](void* u,int,char** v,char**){
            auto* l=(std::vector<Appointment>*)u;
            Appointment a;
            a.id=stoi(v[0]); a.patientId=stoi(v[1]); a.doctorId=stoi(v[2]);
            a.dateTime=v[3]; a.status=v[4]; a.paid=stoi(v[5]);
            l->push_back(a); return 0; }, &list);
    return list;
}