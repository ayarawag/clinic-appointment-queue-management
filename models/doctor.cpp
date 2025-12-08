#include "doctor.h"
#include "../database/db_connection.h"
#include <sstream>
#include <string>
#include <cstdlib>
using std::stoi;

Doctor::Doctor(): id(0) {}
Doctor::Doctor(std::string n,std::string s){ id=0; name=n; specialization=s; }

Doctor Doctor::loadById(int did,const std::string& db){
    Doctor d;
    DBConnection x(db);
    x.query("SELECT id,name,specialization,schedule FROM doctors WHERE id="+std::to_string(did),
    [](void* u,int, char** v,char**){
        Doctor* d=(Doctor*)u;
        d->id=stoi(v[0]); d->name=v[1]; d->specialization=v[2]; d->schedule=v[3];
        return 0; }, &d);
    return d;
}

std::vector<Doctor> Doctor::getAll(const std::string& db){
    std::vector<Doctor> list;
    DBConnection x(db);
    x.query("SELECT id,name,specialization,schedule FROM doctors;",
        [](void* u,int,char** v,char**){
            auto* l=(std::vector<Doctor>*)u;
            Doctor d; d.id=stoi(v[0]); d.name=v[1]; d.specialization=v[2]; d.schedule=v[3];
            l->push_back(d); return 0; }, &list);
    return list;
}

bool Doctor::create(const std::string& db){
    DBConnection x(db);
    std::ostringstream q;
    q<<"INSERT INTO doctors(name,specialization,schedule) VALUES('"
      <<name<<"','"<<specialization<<"','"<<schedule<<"');";
    return x.execute(q.str());
}

bool Doctor::update(const std::string& db){
    DBConnection x(db);
    std::ostringstream q;
    q<<"UPDATE doctors SET name='"<<name<<"', specialization='"<<specialization
      <<"', schedule='"<<schedule<<"' WHERE id="<<id;
    return x.execute(q.str());
}

bool Doctor::remove(const std::string& db){
    DBConnection x(db);
    return x.execute("DELETE FROM doctors WHERE id="+std::to_string(id));
}

bool Doctor::setSchedule(std::string s,const std::string& db){
    schedule=s;
    return update(db);
}