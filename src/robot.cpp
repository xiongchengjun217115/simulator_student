#include "robot.hpp"
#include "team.hpp"
#include "building.hpp"
#include <string>

Robot::Robot(std::string name, Team team, int maxHp, int ammo):
name_(name),team_(team),maxHp_(maxHp),hp_(maxHp),ammo_(ammo),heat_(0),startammo_(ammo_){}

const std::string& Robot::name() const{
    return name_;
}
Team Robot::team() const{
    return team_;
}
int Robot::hp() const{
    return hp_;
}
int Robot::maxHp() const{
    return maxHp_;
}
bool Robot::alive() const{
    return(hp_>0);
}
int Robot::ammo() const{
    return ammo_;
}
int Robot::heat() const{
    return heat_;
}

int Robot::takeDamage(int amount){
    if(amount>hp_)amount=hp_;
    hp_-=amount;
    return amount;
}

void Robot::fire(){
    ammo_--;
    heat_+=heatPerShot();
}

void Robot::cool(int seconds){
    heat_-=coolingPerSec()*seconds;
    if(heat_<0) heat_=0;
}

//附加2
int Robot::heal(int amount){
    if(amount>maxHp_-hp_)amount=maxHp_-hp_;
    hp_+=amount;
    return amount;
}

//附加3
void Robot::respawn(){
    hp_=maxHp_;
    heat_=0;
    ammo_=startammo_;
}

//hero类
Hero::Hero(std::string name, Team team):Robot(name,team,300,10){}

std::string Hero::typeName() const{
    return "hero";
}
int Hero::damagePerShot() const{
    if(mode_=="snipe")return 150;
    return 100;
}
int Hero::heatPerShot() const{
    if(mode_=="snipe")return 150;
    return 100;
}
int Hero::heatLimit() const{
    if(mode_=="snipe")return 300;
    return 200;
}
int Hero::coolingPerSec() const{
    if(mode_=="cooling")return 40;
    return 20;
}

//附加1
ModeControl::Result Hero::requestMode(const std::string& mode){
    if(mode=="standard"||mode=="snipe"||mode=="cooling"){
        if(time_==0){
            if(mode!=mode_){
                target_=mode;
                time_=3;
                return ModeControl::Result::Started;
            }
            return ModeControl::Result::Same;
        }
        return ModeControl::Result::Busy;
    }
    return ModeControl::Result::Unknown;
}
bool Hero::switching() const{
    return (time_>0);
}
std::string Hero::describeMode() const{
    if(time_>0){
        return "switching -> "+target_+" ("+std::to_string(time_)+" s left)";
    }
    return mode_;
} 
std::string Hero::advanceMode(){
    if(time_<=0) return "";
    time_--;
    if(time_==0){
        mode_=target_;
        return mode_;
    }
    return "";
}

//附加3
bool Hero::advanceRespawn(int delay){
    if(alive()){
        deadfor_=0;
        return false;
    }
    deadfor_++;
    if(deadfor_<delay){
        return false;
    }else
    if(deadfor_>=delay){
        respawn();
        deadfor_=0;
        mode_="standard";
        target_.clear();
        time_=0;
        return true;
    }
    return false;
}

//infantry类
Infantry::Infantry(std::string name, Team team):Robot(name,team,200,100){}

std::string Infantry::typeName() const{
    return "infantry";
}
int Infantry::damagePerShot() const{
    if(mode_=="burst")return 15;
    return 10;
}
int Infantry::heatPerShot() const{
    if(mode_=="burst")return 15;
    return 10;
}
int Infantry::heatLimit() const{
    if(mode_=="burst")return 150;
    return 100;
}
int Infantry::coolingPerSec() const{
    if(mode_=="cooling")return 20;
    return 10;
}

//附加1
ModeControl::Result Infantry::requestMode(const std::string& mode){
if(mode=="standard"||mode=="burst"||mode=="cooling"){
        if(time_==0){
            if(mode!=mode_){
                target_=mode;
                time_=3;
                return ModeControl::Result::Started;
            }
            return ModeControl::Result::Same;
        }
        return ModeControl::Result::Busy;
    }
    return ModeControl::Result::Unknown;
}
bool Infantry::switching() const{
    return (time_>0);
}
std::string Infantry::describeMode() const{
    if(time_>0){
        return "switching -> "+target_+" ("+std::to_string(time_)+" s left)";
    }
    return mode_;
} 
std::string Infantry::advanceMode(){
    if(time_<=0) return "";
    time_--;
    if(time_==0){
        mode_=target_;
        return mode_;
    }
    return "";
}

//附加3
bool Infantry::advanceRespawn(int delay){
    if(alive()){
        deadfor_=0;
        return false;
    }
    deadfor_++;
    if(deadfor_<delay){
        return false;
    }else
    if(deadfor_>=delay){
        respawn();
        deadfor_=0;
        mode_="standard";
        target_.clear();
        time_=0;
        return true;
    }
    return false;
}

//附加2
Engineer::Engineer(std::string name,Team team):Robot(name,team,250,0){}

std::string Engineer::typeName() const{
    return "engineer";
} 
int Engineer::damagePerShot() const{
    return 0;
} 
int Engineer::heatPerShot() const{
    return 0;
} 
int Engineer::heatLimit() const{
    return 0;
} 
int Engineer::coolingPerSec() const{
    return 0;
} 

bool Engineer::ready() const{
    return (time_==0);
}              

int Engineer::repair(Robot& target){
    if(time_>0) return 0;
    time_=5;
    return target.heal(50);
}       

int Engineer::repair(Building& target){
    if(time_>0) return 0;
    time_=5;
    return target.heal(50);
}  

void Engineer::advanceRepair(){
    if(time_>0) time_--;
}          
