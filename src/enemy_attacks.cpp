#include "enemy_attacks.hpp"

std::vector<Attack> getAvailableAttacksForEnemyType(const std::string& enemyType) {
    std::vector<Attack> attacks;

    // How to add new attacks:
    // name(n), damage(dmg), manaCost(mana), affectedEnemy(affectedEnemy), hits(hits), heal(heal)
    if (enemyType == "Melee") {
        attacks.push_back(Attack{ "Punch", 5, 10, 1, 1, 0, false});    // Attack name, damage, mana cost, number of hits
        attacks.push_back(Attack{ "Slash", 15, 20, 2, 1, 0, false });
        attacks.push_back(Attack{ "Heavy Hit", 20, 30, 1, 2, 0, false });
    }
    else if (enemyType == "Ranged") {
        attacks.push_back(Attack{ "Star Shot", 10, 5, 1, 1, 0, true });
        attacks.push_back(Attack{ "Fireball", 20, 25, 1, 1, 0, true });
        attacks.push_back(Attack{ "Ice Spear", 30, 15, 1, 1, 0, true });
    }
    else if (enemyType == "Healer") {
        attacks.push_back(Attack{ "Heal", 0, 10, 1, 1, 50, false });
        attacks.push_back(Attack{ "Fireball", 20, 25, 1, 1, 0, true });
        attacks.push_back(Attack{ "Ice Spear", 30, 15, 1, 1, 0, true });
    }
    else if (enemyType == "Boss") {
        attacks.push_back(Attack{ "Self Heal", 0, 10, 1, 1, 50, false });
        attacks.push_back(Attack{ "Big Fireball", 50, 25, 1, 1, 0, true });
        attacks.push_back(Attack{ "Very Big Fireball", 100, 15, 1, 1, 0, true });
    }
    else if (enemyType == "Tank") {
        attacks.push_back(Attack{ "Punch", 10, 0, 1, 1, 50, false });
        attacks.push_back(Attack{ "Heavy Punch", 20, 20, 1, 1, 0, false });
    }

    return attacks;
}