#include "game.hpp"

#include <set>
#include <utility>

namespace {
std::optional<Team> parseTeam(const std::string& s) {
    if (s == "red") return Team::Red;
    if (s == "blue") return Team::Blue;
    return std::nullopt;
}

// 目标可能是机器人，也可能是建筑。两者没有共同父类，这里统一包一下。
struct Target {
    Robot* robot = nullptr;
    Building* building = nullptr;

    explicit operator bool() const { return robot || building; }
    bool alive() const { return robot ? robot->alive() : building->alive(); }
    Team team() const { return robot ? robot->team() : building->team(); }
    int hp() const { return robot ? robot->hp() : building->hp(); }
    int maxHp() const { return robot ? robot->maxHp() : building->maxHp(); }
    std::string typeName() const { return robot ? robot->typeName() : building->typeName(); }
    int takeDamage(int amount) {
        return robot ? robot->takeDamage(amount) : building->takeDamage(amount);
    }
};
template <class Map>
auto* lookup(Map& m, const std::string& name) {
    auto it = m.find(name);
    return it == m.end() ? nullptr : it->second.get();
}

}  // namespace

std::ostream& Game::stamp() const { return out_ << "[t=" << time_ << "] "; }

bool Game::exists(const std::string& name) const {
    return robots_.count(name) || buildings_.count(name);
}

void Game::registerType(const std::string& type, Factory factory) {
    factories_[type] = std::move(factory);
}

void Game::spawn(const std::string& name, const std::string& type, const std::string& teamStr) {
    if (over()) {
        out_ << "error: game over\n";
        return;
    }
    auto team = parseTeam(teamStr);
    if (!team) {
        out_ << "error: unknown team " << teamStr << "\n";
        return;
    }

    // 整个程序里唯一按类型字符串创建对象的地方。
    if (type == "hero") {
        robots_[name] = std::make_unique<Hero>(name, *team);
    } else if (type == "infantry") {
        robots_[name] = std::make_unique<Infantry>(name, *team);
    } else if (type == "outpost") {
        auto outpost = std::make_unique<Outpost>(name, *team);
        outposts_[*team] = outpost.get();
        buildings_[name] = std::move(outpost);
    } else if (type == "base") {
        auto it = outposts_.find(*team);
        buildings_[name] =
            std::make_unique<Base>(name, *team, it == outposts_.end() ? nullptr : it->second);
    } else if (auto it = factories_.find(type); it != factories_.end()) {
        robots_[name] = it->second(name, *team);  // 选做：通过 registerType 注册的类型
    } else {
        out_ << "error: unknown type " << type << "\n";
        return;
    }

    if (robots_.count(name)) stats_[name];
    stamp() << "spawn " << name << " " << type << " " << teamStr << "\n";
}

void Game::shoot(const std::string& shooterName, const std::string& targetName) {
    if (over()) {
        out_ << "error: game over\n";
        return;
    }

    // 1. 名字必须存在
    if (!exists(shooterName)) {
        out_ << "error: no such entity " << shooterName << "\n";
        return;
    }
    if (!exists(targetName)) {
        out_ << "error: no such entity " << targetName << "\n";
        return;
    }

    // 2. 开火方必须是机器人
    Robot* robot = lookup(robots_, shooterName);
    if (!robot) {
        out_ << "error: " << shooterName << " is not a robot\n";
        return;
    }
    Target target{lookup(robots_, targetName), lookup(buildings_, targetName)};
    auto* modes = dynamic_cast<ModeControl*>(robot);

    // 3 ~ 7. 依次检查（选做 1 在 dead 之后多一条 switching）
    const char* reason = nullptr;
    if (!robot->alive()) reason = "dead";
    else if (modes && modes->switching()) reason = "switching";
    else if (!target.alive()) reason = "target down";
    else if (robot->team() == target.team()) reason = "same team";
    else if (robot->ammo() <= 0) reason = "no ammo";
    else if (robot->heat() + robot->heatPerShot() > robot->heatLimit()) reason = "overheat";
    if (reason) {
        stamp() << shooterName << " cannot shoot: " << reason << "\n";
        return;
    }

    // 开火并结算伤害。基地是否受保护由 Base::takeDamage 自己决定。
    robot->fire();
    int dealt = target.takeDamage(robot->damagePerShot());

    if (dealt == 0) {
        stamp() << shooterName << " -> " << targetName << " protected, hp " << target.hp() << "\n";
        return;
    }
    stamp() << shooterName << " -> " << targetName << " " << dealt << " damage, hp "
            << target.hp() << "\n";
    stats_[shooterName].damage += dealt;

    if (!target.alive()) {
        ++stats_[shooterName].kills;
        stamp() << targetName << " destroyed\n";
        if (dynamic_cast<Base*>(target.building)) {  // 基地被打掉，对方获胜
            winner_ = robot->team();
            stamp() << "winner: " << teamName(*winner_) << "\n";
        }
    }
}

void Game::tick(int seconds) {
    if (over()) {
        out_ << "error: game over\n";
        return;
    }
    if (seconds < 0) {
        out_ << "error: bad argument for tick\n";
        return;
    }
    time_ += seconds;

    // tick n 等价于连续 n 次“前进 1 秒”。
    std::map<std::string, std::string> modeReady;
    std::set<std::string> respawned;
    for (int s = 0; s < seconds; ++s) {
        for (auto& [name, robot] : robots_) {
            robot->cool(1);
            if (auto* modes = dynamic_cast<ModeControl*>(robot.get())) {
                std::string done = modes->advanceMode();
                if (!done.empty() && robot->alive()) modeReady[name] = done;
            }
            if (auto* repairer = dynamic_cast<Repairer*>(robot.get())) repairer->advanceRepair();
            if (respawnDelay_ > 0) {
                if (auto* rs = dynamic_cast<Respawnable*>(robot.get())) {
                    if (rs->advanceRespawn(respawnDelay_)) respawned.insert(name);
                }
            }
        }
    }
    stamp() << "tick " << seconds << "\n";
    for (const auto& [name, mode] : modeReady) stamp() << name << " mode " << mode << " ready\n";
    for (const auto& name : respawned) stamp() << name << " respawned\n";
}

void Game::status() const {
    // 先机器人，再建筑；map 本身就是字典序。
    for (const auto& [name, r] : robots_) {
        out_ << name << " " << r->typeName() << " " << teamName(r->team()) << " hp " << r->hp()
             << "/" << r->maxHp() << " ammo " << r->ammo() << " heat " << r->heat() << "\n";
    }
    for (const auto& [name, b] : buildings_) {
        out_ << name << " " << b->typeName() << " " << teamName(b->team()) << " hp " << b->hp()
             << "/" << b->maxHp() << "\n";
    }
}

void Game::report() const {
    for (const auto& [name, stat] : stats_) {
        out_ << name << " damage " << stat.damage << " kills " << stat.kills << "\n";
    }
}

// ---------------- 选做命令 ----------------

void Game::mode(const std::string& name, const std::optional<std::string>& wanted) {
    if (wanted) {  // 切换会改变状态；查询不受 game over 影响
        if (over()) {
            out_ << "error: game over\n";
            return;
        }
        }

    if (!exists(name)) {
        out_ << "error: no such entity " << name << "\n";
        return;
    }
    Robot* robot = lookup(robots_, name);
    if (!robot) {
        out_ << "error: " << name << " is not a robot\n";
        return;
    }
    auto* modes = dynamic_cast<ModeControl*>(robot);
    if (!modes) {
        out_ << "error: " << name << " has no modes\n";
        return;
    }

    if (!wanted) {
        out_ << name << " mode " << modes->describeMode() << "\n";
        return;
    }
    if (!robot->alive()) {
        stamp() << name << " cannot switch: dead\n";
        return;
    }
    switch (modes->requestMode(*wanted)) {
        case ModeControl::Result::Unknown:
            out_ << "error: unknown mode " << *wanted << "\n";
            break;
        case ModeControl::Result::Busy:
            stamp() << name << " cannot switch: switching\n";
            break;
        case ModeControl::Result::Same:
            stamp() << name << " cannot switch: same mode\n";
            break;
        case ModeControl::Result::Started:
            stamp() << name << " switching to " << *wanted << "\n";
            break;
    }
}

void Game::repair(const std::string& healerName, const std::string& targetName) {
    if (over()) {
        out_ << "error: game over\n";
        return;
    }

    if (!exists(healerName)) {
        out_ << "error: no such entity " << healerName << "\n";
        return;
    }
    if (!exists(targetName)) {
        out_ << "error: no such entity " << targetName << "\n";
        return;
    }
    Robot* healer = lookup(robots_, healerName);
    auto* repairer = dynamic_cast<Repairer*>(healer);
    if (!repairer) {
        out_ << "error: " << healerName << " cannot repair\n";
        return;
    }
    Target target{lookup(robots_, targetName), lookup(buildings_, targetName)};

    const char* reason = nullptr;
    if (!healer->alive()) reason = "dead";
    else if (!target.alive()) reason = "target down";
    else if (healer->team() != target.team()) reason = "other team";
    else if (target.hp() == target.maxHp()) reason = "already full";
    else if (!repairer->ready()) reason = "cooling down";
    if (reason) {
        stamp() << healerName << " cannot repair: " << reason << "\n";
        return;
    }

    int healed = target.robot ? repairer->repair(*target.robot) : repairer->repair(*target.building);
    stamp() << healerName << " -> " << targetName << " repaired " << healed << ", hp "
            << target.hp() << "\n";
}

void Game::rule(const std::string& key, const std::string& value) {
    if (over()) {
        out_ << "error: game over\n";
        return;
    }

    if (key == "respawn") {
        int delay = -1;
        try {
            std::size_t used = 0;
            delay = std::stoi(value, &used);
            if (used != value.size()) delay = -1;
        } catch (const std::exception&) {
            delay = -1;
        }
        if (delay < 0) {
            out_ << "error: bad value " << value << "\n";
            return;
        }
        respawnDelay_ = delay;
    } else {
        out_ << "error: unknown rule " << key << "\n";
        return;
    }
    stamp() << "rule " << key << " " << value << "\n";
}
