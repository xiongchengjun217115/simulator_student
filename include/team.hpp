#pragma once

#include <string>

enum class Team { Red, Blue };

inline std::string teamName(Team t) { return t == Team::Red ? "red" : "blue"; }
