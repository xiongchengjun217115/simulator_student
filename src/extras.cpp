// 做附加任务时才需要改这个文件。

#include <memory>
#include <ostream>
#include <string>

#include "extensions.hpp"
#include "game.hpp"

// 选做 2（工程）要用到这里；选做 1、3 不用动这个文件。
//
// 注册的意思：告诉 Game "输入里出现 spawn 名字 engineer 队伍 时，用下面这段代码创建对象"。
// 我已经写好了。你写完 Engineer 类（写在 robot.hpp / robot.cpp 里即可）后，
// 把下面三行前面的 // 去掉就行。Engineer 的构造函数要能接收 (name, team)。
void registerExtras([[maybe_unused]] Game& game) {
    game.registerType("engineer", [](const std::string& name, Team team) {
         return std::make_unique<Engineer>(name, team);
    });
}

