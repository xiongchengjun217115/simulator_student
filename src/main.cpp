#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include "extensions.hpp"
#include "game.hpp"

int main() {
    Game game(std::cout);
    registerExtras(game);

    std::string line;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string cmd;
        if (!(in >> cmd)) continue;  // 空行

        auto bad = [&] { std::cout << "error: bad arguments for " << cmd << "\n"; };

        if (cmd == "spawn") {
            std::string name, type, team;
            if (in >> name >> type >> team) game.spawn(name, type, team); else bad();
        } else if (cmd == "shoot") {
            std::string shooter, target;
            if (in >> shooter >> target) game.shoot(shooter, target); else bad();
        } else if (cmd == "tick") {
            int seconds = 0;
            if (in >> seconds) game.tick(seconds); else bad();
        } else if (cmd == "status") {
            game.status();
        } else if (cmd == "report") {
            game.report();
        } else if (cmd == "mode") {
            std::string name, wanted;
            if (!(in >> name)) { bad(); continue; }
            if (in >> wanted) game.mode(name, wanted); else game.mode(name, std::nullopt);
        } else if (cmd == "repair") {
            std::string healer, target;
            if (in >> healer >> target) game.repair(healer, target); else bad();
        } else if (cmd == "rule") {
            std::string key, value;
            if (in >> key >> value) game.rule(key, value); else bad();
        } else {
            std::cout << "error: unknown command " << cmd << "\n";
        }
    }
    return 0;
}
