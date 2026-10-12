#pragma once

#include <string>
#include "team.hpp"

class Building{
public:
Building(std::string name, Team team, int maxHp);
virtual ~Building()=default;

const std::string& name() const;
Team team() const;
int hp() const;
int maxHp() const;
bool alive() const;

virtual int takeDamage(int amount);  // 返回实际扣掉的血量
virtual std::string typeName() const = 0;  // 返回命令里的名字：Outpost 返回 "outpost"，Base 返回 "base"

//附加2
int heal(int amount);

private:
std::string name_;
Team team_;
int maxHp_;
int hp_;
};

class Outpost:public Building{
public:
std::string typeName() const override;
Outpost(std::string name, Team team);
int takeDamage(int amount)override;
private:
};

class Base:public Building{
public:
std::string typeName() const override;
Base(std::string name, Team team, const Outpost* outpost);  // 可能是 nullptr
int takeDamage(int amount)override;
private:
const Outpost* outpost_=nullptr;
};
