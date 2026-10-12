#pragma once

#include <string>
#include "team.hpp"
#include "extensions.hpp"

class Robot{
public:
Robot(std::string name, Team team, int maxHp, int ammo);
virtual ~Robot()=default;

const std::string& name() const;
Team team() const;
int hp() const;
int maxHp() const;
bool alive() const;
int ammo() const;
int heat() const;

int takeDamage(int amount);          // 返回实际扣掉的血量
void fire();
void cool(int seconds);

virtual std::string typeName() const = 0;  // 返回命令里的名字：Hero 返回 "hero"，Infantry 返回 "infantry"（附加 2 的 Engineer 返回 "engineer"）
virtual int damagePerShot() const = 0;
virtual int heatPerShot() const = 0;
virtual int heatLimit() const = 0;
virtual int coolingPerSec() const = 0;

//附加2
int heal(int amount);

//附加3
void respawn();

private:
std::string name_;
Team team_;
int maxHp_;
int ammo_;
int hp_;
int heat_;
int startammo_;
};

class Hero:public Robot,public ModeControl,public Respawnable{
public:
Hero(std::string name, Team team);
std::string typeName() const override;
int damagePerShot() const override;
int heatPerShot() const override;
int heatLimit() const override;
int coolingPerSec() const override;

//附加1
Result requestMode(const std::string& mode) override;
bool switching() const override;
std::string describeMode() const override;
std::string advanceMode() override;

//附加3
bool advanceRespawn(int delay)override;

private:
std::string mode_="standard";
std::string target_;
int time_=0;
int deadfor_=0;
};

class Infantry:public Robot,public ModeControl,public Respawnable{
public:
Infantry(std::string name, Team team);
std::string typeName() const override;
int damagePerShot() const override;
int heatPerShot() const override;
int heatLimit() const override;
int coolingPerSec() const override;

//附加1
Result requestMode(const std::string& mode) override;
bool switching() const override;
std::string describeMode() const override;
std::string advanceMode() override;

//附加3
bool advanceRespawn(int delay)override;

private:
std::string mode_="standard";
std::string target_;
int time_=0;
int deadfor_=0;
};

//附加2
class Engineer:public Robot,public Repairer{
public:
Engineer(std::string name,Team team);
virtual ~Engineer() = default;

std::string typeName() const override;
int damagePerShot() const override;
int heatPerShot() const override;
int heatLimit() const override;
int coolingPerSec() const override;

bool ready() const override;             // 冷却完毕，可以再次修理
int repair(Robot& target) override;      // 修理并开始冷却，返回实际恢复的血量
int repair(Building& target)override;   // 同上，目标是建筑
void advanceRepair()override;           // 前进 1 秒

private:
int time_=0;
};