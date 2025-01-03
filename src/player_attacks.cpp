#include <player_attacks.hpp>
#include <stdexcept>

// name(n), damage(dmg), manaCost(mana), affectedEnemy(affectedEnemy), hits(hits), heal(heal)
Attack getPlayerAttack(const std::string& attackName) {
    static const std::unordered_map<std::string, Attack> attackMap = {
        {"Punch", {"Punch", 5, 10, 1, 3, 0, false}},
        {"Slash", {"Slash", 15, 20, 2, 2, 0, false}},
        {"Heavy Hit", {"Heavy Hit", 20, 30, 1, 2, 0, false}},
        {"Arrow Shot", {"Arrow Shot", 10, 5, 1, 1, 0, true}},
        {"Fireball", {"Fireball", 20, 25, 1, 1, 0, true}},
        {"Ice Spear", {"Ice Spear", 30, 15, 1, 1, 0, true}},
        {"Heal", {"Heal", 0, 50, 0, 0, 50, false}}
    };

    auto it = attackMap.find(attackName);
    if (it != attackMap.end()) {
        return it->second;
    }

    throw std::invalid_argument("Attack not found: " + attackName);
}