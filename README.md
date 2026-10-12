# 作业：RoboMaster 赛场模拟器

**前言**：一个终端里的赛场模拟器，读命令（生成机器人和建筑、开火、推进时间），按规则模拟场上血量、弹量、热量的变化，直到一方基地被摧毁。读命令和打印我已经写好了（`Game`、`main`），你写场上的机器人和建筑。

## 一、任务概述

1. 写机器人类：`Robot`、`Hero`、`Infantry`
   `Hero`、`Infantry` 继承于 `Robot`
2. 写建筑类：`Building`、`Outpost`、`Base`
   `Outpost`、`Base` 继承于 `Building`
3. 写 `CMakeLists.txt`

你写 `robot.*`、`building.*` 和 `CMakeLists.txt`，其余文件都是我给的。规则和命令格式见 [docs/rules.md](docs/rules.md)，先看。

## 二、详细要求

下面的函数，名字、参数、返回类型照抄，`Game` 会调用。你可以写自己命名的函数来帮助处理数据，但`Game`不会调用。

### 1. 机器人

**功能**：会开火的机器人。英雄和步兵只有血量等数字不同。

```cpp
Robot(std::string name, Team team, int maxHp, int ammo);

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

Hero(std::string name, Team team);
Infantry(std::string name, Team team);
```

**要求**：

- 英雄和步兵的数字不同（见规则里的常量表）。请在 `Hero`、`Infantry` 里各自重写上面的虚函数，不要在 `Robot` 里写 `if (type == "hero")` 这样的判断
- 数据成员放在 `private`；`Robot` 的析构函数要写成 `virtual`（否则通过 `Robot*` 删除子类对象时，子类部分不会被正确清理）
- 有没有弹、会不会过热由 `Game` 判断

### 2. 建筑

**功能**：不会开火、只会被打。

```cpp
Building(std::string name, Team team, int maxHp);

const std::string& name() const;
Team team() const;
int hp() const;
int maxHp() const;
bool alive() const;

virtual int takeDamage(int amount);  // 返回实际扣掉的血量
virtual std::string typeName() const = 0;  // 返回命令里的名字：Outpost 返回 "outpost"，Base 返回 "base"

Outpost(std::string name, Team team);
Base(std::string name, Team team, const Outpost* outpost);  // 可能是 nullptr
```

**要求**：

- 前哨站保护基地的规则（见 `docs/rules.md` 第 5 条）请你自己在 `Base` 里实现：`Base` 拿到前哨站的指针，被攻击时自己判断前哨站是否还在，`Game` 不处理这件事
- 成员私有，析构函数是虚函数

### 3. CMakeLists.txt

- 库 `simulator_core`：所有 `.cpp`，不含 `main.cpp`
- 可执行程序 `simulator`：`main.cpp`，链接 `simulator_core`
- `include/` 是 `simulator_core` 的 PUBLIC 头文件目录
- C++17

能用下面两条命令构建出 `build/simulator`：

```bash
cmake -S . -B build
cmake --build build
```

## 三、附加任务

做完基础部分再看，互相独立。不要改 `Game`、`main.cpp`、`extensions.hpp`。

1. [模式切换（状态机）](docs/extra/1-mode.md)
2. [工程和修理](docs/extra/2-repair.md)
3. [复活](docs/extra/3-respawn.md)

## 四、自测与要求

`data/` 里有 6 个输入，都带标准答案：

- `01`、`02`：基础部分。**只做基础的话，这两个 PASS 就行**
- `03` 到 `05`：分别对应附加 1 到 3，没做对应的附加任务就是 FAIL，正常
- `06`：全部功能一起，要做完所有附加任务才会 PASS

```bash
./run_all.sh              # 全部跑一遍，显示 PASS / FAIL，不一致的会列出哪几行
./run_all.sh 02_win       # 只跑一个，直接打印在屏幕上
```

评分会用 `data/` 里的输入，再加一些没给出来的。

此次作业禁止使用codex claude opencode等agent ，可以使用网页版ai（仅聊天，不可以把要求直接给他让他直接生成），如果在检查作业时候发现疑似纯ai生成，会给很低的分
基础题还是比较简单的，不管怎么样，大家哪怕基础题都没做好也尽量在截止时间交一版上来，没交直接0分
