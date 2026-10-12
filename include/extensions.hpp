#pragma once

// 选做题用到的"功能接口"（我提供的，不要修改）。
//
// 这个文件定义了三个类：ModeControl、Repairer、Respawnable。
// 它们只是"功能清单"，所以也叫接口：里面只列出"要支持这个功能，必须有哪些函数"，
// 不写函数怎么实现。怎么实现，由你在自己的类里写。
//
// 怎么用：你的类在原来继承 Robot（或 Building）的基础上，再多继承其中一个类。
//
//     class Hero : public Robot {};                    // 基础版：只是机器人
//     class Hero : public Robot, public ModeControl {};// 选做 1：还支持切换模式
//
// 继承后，必须把这个类里的所有函数都写出来（写在类里，并加 override），
// 否则编译不通过。
//
// Game 不需要你改。它处理命令时，会先检查这个对象有没有继承对应的类
// （用 dynamic_cast）：
//   - 继承了：调用你写的函数；
//   - 没继承：当作"不支持"，直接跳过。
// 所以不做选做题，基础版完全不受影响。
//
// 各选做题要继承的类：
//   选做 1 模式切换 -> ModeControl（英雄、步兵）
//   选做 2 修理     -> Repairer（工程）
//   选做 3 复活     -> Respawnable（英雄、步兵）

#include <functional>
#include <ostream>
#include <string>

class Game;
class Robot;
class Building;

// 选做 1：兵种模式。
class ModeControl {
public:
    virtual ~ModeControl() = default;

    enum class Result { Started, Unknown, Busy, Same };

    // 请求切换到 mode。检查顺序：模式不存在(Unknown) -> 正在切换(Busy) -> 已是该模式(Same)，
    // 都通过则开始切换(Started)，需要 3 秒。
    virtual Result requestMode(const std::string& mode) = 0;
    // 是否正处于切换过程中（此时不能开火）。
    virtual bool switching() const = 0;
    // 查询用的描述："standard" 或 "switching -> snipe (1 s left)"。
    virtual std::string describeMode() const = 0;
    // 前进 1 秒。恰好在这一秒切换完成时返回新模式名，否则返回空串。
    virtual std::string advanceMode() = 0;
};

// 选做 2：修理。目标可能是机器人，也可能是建筑，所以有两个版本。
class Repairer {
public:
    virtual ~Repairer() = default;

    virtual bool ready() const = 0;             // 冷却完毕，可以再次修理
    virtual int repair(Robot& target) = 0;      // 修理并开始冷却，返回实际恢复的血量
    virtual int repair(Building& target) = 0;   // 同上，目标是建筑
    virtual void advanceRepair() = 0;           // 前进 1 秒
};

// 选做 3：复活。只有在比赛中开启了 `rule respawn N` 时，Game 才会调用它。
class Respawnable {
public:
    virtual ~Respawnable() = default;

    // 前进 1 秒。如果已被摧毁，并且已经连续被摧毁 delay 秒，就复活并返回 true。
    virtual bool advanceRespawn(int delay) = 0;
};

// ---- 入口：registerExtras ----
//
// 选做 1、3 不需要动它：英雄、步兵本来就能被创建，只要让它们多继承一个类就行。
// 选做 2 要用它：工程是全新的兵种，Game 不认识 "engineer"，需要你在这里告诉它
// "遇到 engineer 时，这样创建对象"。
//
// 注册的代码我已经写在 src/extras.cpp 里，是注释状态。
// 你写完 Engineer 类后，把那几行前面的 // 去掉就行。
// 它返回一个新建的 Engineer，Game 把它放进机器人列表里。
//
// 写完后编译、运行对应的输入文件（选做 2 是 data/04_repair）检查结果。
void registerExtras(Game& game);
