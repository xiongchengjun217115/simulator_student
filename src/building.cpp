#include "building.hpp"
#include <string>

Building::Building(std::string name, Team team, int maxHp):name_(name),team_(team),maxHp_(maxHp),hp_(maxHp){}

const std::string &Building::name() const{
    return name_;
}
Team Building::team() const{
    return team_;
}
int Building::hp() const{
    return hp_;
}
int Building::maxHp() const{
    return maxHp_;
}
bool Building::alive() const{
   return (hp_>0);
}

int Building::takeDamage(int amount){
    if(amount>hp_)amount=hp_;
    hp_-=amount;
    return amount;
}

//附加2
int Building::heal(int amount){
    if(amount>maxHp_-hp_)amount=maxHp_-hp_;
    hp_+=amount;
    return amount;
}

//outpost类
Outpost::Outpost(std::string name, Team team):Building(name,team,300){}

std::string Outpost::typeName() const{
    return "outpost";
}

int Outpost::takeDamage(int amount){
    return Building::takeDamage(amount);
}

//base类
Base::Base(std::string name, Team team, const Outpost* outpost):Building(name,team,600),outpost_(outpost){}

std::string Base::typeName() const{
    return "base";
}

int Base::takeDamage(int amount){
    if(outpost_ && outpost_->alive()){
        return 0;
    }
    else return Building::takeDamage(amount);
}
