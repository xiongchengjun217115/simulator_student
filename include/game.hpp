#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "building.hpp"
#include "extensions.hpp"
#include "robot.hpp"

// 一场比赛：保存时间、所有机器人和建筑、战绩，处理每一条命令。
class Game {
public:
    explicit Game(std::ostream& out) : out_(out) {}

    // ---- 基础命令 ----
    void spawn(const std::string& name, const std::string& type, const std::string& team);
    void shoot(const std::string& shooter, const std::string& target);
    void tick(int seconds);
    void status() const;
    void report() const;

    // ---- 选做命令（对象没有实现对应的扩展接口时，会打印 error） ----
    void mode(const std::string& name, const std::optional<std::string>& wanted);  // 选做 1
    void repair(const std::string& healer, const std::string& target);             // 选做 2
    void rule(const std::string& key, const std::string& value);                   // 选做 3

    // ---- 选做入口，供 registerExtras 使用 ----
    using Factory = std::function<std::unique_ptr<Robot>(const std::string& name, Team team)>;
    void registerType(const std::string& type, Factory factory);  // 注册新的机器人类型

    bool over() const { return winner_.has_value(); }

private:
    struct Stat {
        int damage = 0;
        int kills = 0;
    };

    std::ostream& stamp() const;  // 输出 "[t=当前时间] " 并返回 out_
    bool exists(const std::string& name) const;

    std::ostream& out_;
    int time_ = 0;
    int respawnDelay_ = 0;  // rule respawn N；0 表示关闭
    std::optional<Team> winner_;
    std::map<std::string, std::unique_ptr<Robot>> robots_;        // 按名字字典序
    std::map<std::string, std::unique_ptr<Building>> buildings_;  // 按名字字典序
    std::map<std::string, Stat> stats_;
    std::map<Team, const Outpost*> outposts_;  // 每方的前哨站

    std::map<std::string, Factory> factories_;  // 通过 registerType 加入的类型
};
