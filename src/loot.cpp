#include "loot.hpp"
#include <vector>
#include <string>
#include <functional>
#include <unordered_set>
#include "components.hpp" 
#include "world_system.hpp" 
#include <random>

extern Entity hero;

// possible new attacks
// name(n), damage(dmg), manaCost(mana), affectedEnemy(affectedEnemy), hits(hits), heal(heal)
const std::vector<Attack> attackPool = {
    { "Fireball", 50, 30, 1, 1, 0, true },
    { "Lightning", 60, 35, 2, 1, 0, true },
    { "IceShard", 40, 25, 3, 1, 0, true },
    { "Zap", 100, 50, 1, 1, 0, false },
    { "Quake", 50, 5, 1, 1, 0, false },
    { "Lifetap", 20, 10, 1, 1, 20, true }
};

// Future: add more
const std::vector<int> damageModifierPool = { 10, 25, 50 }; 

// Future: add more
const std::vector<int> manaModifierPool = { 10, 20, 30 }; 

// Future: add more
const std::vector<int> healthIncreasePool = { 10, 20, 30 };

std::unordered_set<std::string> chosenAttacks;

#include <random>

// Return a reference to the selected attack
Attack& selectRandomAttack(Entity hero) {
    if (!registry.attacks.has(hero)) {
        throw std::runtime_error("Hero has no attacks available.");
    }

    // Retrieve the player's available attacks
    auto& availableAttacks = registry.attacks.get(hero).availableAttacks;

    if (availableAttacks.empty()) {
        throw std::runtime_error("No available attacks to select.");
    }

    // Randomly select one of the attacks
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<size_t> dist(0, availableAttacks.size() - 1);

    size_t selectedIndex = dist(rng);

    // Return a reference to the randomly selected attack
    return availableAttacks[selectedIndex];
}

std::vector<LootOption> getAvailableLootOptions() {
    static std::random_device rd;
    static std::mt19937 rng(rd());

    // Filter so we don't repeat attacks
    std::vector<Attack> availableAttacks;
    for (const auto& attack : attackPool) {
        if (chosenAttacks.find(attack.name) == chosenAttacks.end()) {
            availableAttacks.push_back(attack);
        }
    }

    // Should not come to this point
    if (availableAttacks.empty()) {
        printf("All attacks from the pool have been chosen. No more attack options available.\n");
    }

    // Randomly select a new attack from the filtered pool
    Attack randomAttack = { "None", 0, 0, 0, 0, 0, false }; // If somehow no more attacks
    if (!availableAttacks.empty()) {
        std::uniform_int_distribution<size_t> attackDist(0, availableAttacks.size() - 1);
        randomAttack = availableAttacks[attackDist(rng)];
    }
    std::string randomAttackStats = "Damage: " + std::to_string(randomAttack.damage) +
                            ", Mana Cost: " + std::to_string(randomAttack.manaCost) +
                            ", Heal: " + std::to_string(randomAttack.heal);

    // Randomly select a health or mana increase from the pools
    std::uniform_int_distribution<size_t> healthDist(0, healthIncreasePool.size() - 1);
    int randomHealthIncrease = healthIncreasePool[healthDist(rng)];
    int randomManaIncrease = healthIncreasePool[healthDist(rng)]; // Using the same pool for simplicity

    // Randomly choose whether the reward is health or mana
    bool isHealthIncrease = std::uniform_int_distribution<int>(0, 1)(rng);

    int randomNumber2 = 0;

    std::string modifiedAttackStatement = "";
    int randomNumber = 0;
    int randomModifier = 0;

    Attack& selectedAttack = selectRandomAttack(hero);

    // Check if the attack has a healing value greater than 0
    if (selectedAttack.heal > 0) {
        if (selectedAttack.manaCost == 0 ) {
            std::uniform_int_distribution<int> dist(0, 1); 
            randomNumber = dist(rng) == 0 ? 1 : 3; 
        } else {
            std::uniform_int_distribution<int> dist(1, 3);
            randomNumber = dist(rng); 
        }
    } else {
        if (selectedAttack.manaCost <= 5){
            randomNumber = 1;
        } else {
            std::uniform_int_distribution<int> dist(1, 2);
            randomNumber = dist(rng); 
        }
    }
    
    switch (randomNumber) {
        case 1: {
            // Increase damage of an attack
            std::uniform_int_distribution<size_t> modifierDist(0, damageModifierPool.size() - 1);
            randomModifier = damageModifierPool[modifierDist(rng)];
            modifiedAttackStatement = "Increase damage of " + selectedAttack.name + " by " + std::to_string(randomModifier);
            break;
        }
        case 2: {
            // Reduce mana cost
            std::uniform_int_distribution<size_t> modifierDist(0, manaModifierPool.size() - 1);
            randomModifier = manaModifierPool[modifierDist(rng)];
            int newMana = selectedAttack.manaCost - randomModifier;
            if (newMana < 0 ) {
                newMana = 0;
            }
            modifiedAttackStatement = "Reduce the mana cost of " + selectedAttack.name + " to " + std::to_string(newMana);
            break;
        }
        case 3: {
            // Increase healing
            std::uniform_int_distribution<size_t> modifierDist(0, healthIncreasePool.size() - 1);
            randomModifier = healthIncreasePool[modifierDist(rng)];
            modifiedAttackStatement = "Increase the healing of " + selectedAttack.name + " by " + std::to_string(randomModifier);
            break;
        }
        default:
            printf("Unexpected case: %d\n", randomNumber);
            break;
    }

    std::uniform_int_distribution<int> dist(0, 1); 
    randomNumber2 = dist(rng);
    std::string healthStatement = "";
    std::string healthTitle = "";
    if (randomNumber2 == 0) {
        healthStatement = "Increase maximum health by " + std::to_string(randomHealthIncrease) + ".";
        healthTitle = "Increase Maximum Health";
    } else if (randomNumber2 == 1) {
        healthStatement = "Increase maximum mana by " + std::to_string(randomManaIncrease) + ".";
        healthTitle = "Increase Maximum Mana";
    }


    return {
        // Return an attack
        {
            "New Attack",
            availableAttacks.empty() ? "No new attacks available!" : randomAttack.name + ": " + randomAttackStats,
            nullptr,
            [randomAttack]() -> Attack {
                if (randomAttack.name != "None") {
                    printf("Returning new attack: %s!\n", randomAttack.name.c_str());
                    chosenAttacks.insert(randomAttack.name); // Mark this attack as chosen
                }
                return randomAttack;
            }
        },
        // Increase attack damage
        {
            "Level Up Attack",
            modifiedAttackStatement + ".",
            [randomNumber, randomModifier, &selectedAttack]() mutable { 
                switch(randomNumber){
                    case 1: {
                        selectedAttack.damage += randomModifier;
                        printf("Increased damage of %s by %d. New damage: %d\n", 
                            selectedAttack.name.c_str(), randomModifier, selectedAttack.damage);
                        break;
                    }  
                    case 2: {
                        selectedAttack.manaCost = std::max(0, selectedAttack.manaCost - randomModifier);
                        printf("Reduced mana cost of %s by %d. New mana cost: %d\n", 
                            selectedAttack.name.c_str(), randomModifier, selectedAttack.manaCost);
                        break;
                    }
                    case 3: {
                        selectedAttack.heal += randomModifier;
                        printf("Increased healing of %s by %d. New healing: %d\n", 
                            selectedAttack.name.c_str(), randomModifier, selectedAttack.heal);
                        break;
                    }
                    default:
                        break;
                }
            },
            nullptr
        },
        // Randomly increase health or mana
        {
            healthTitle,
            healthStatement,
            [randomNumber2, randomHealthIncrease, randomManaIncrease]() { 
                if (randomNumber2 == 0) {
                    printf("Increasing max health by %d!\n", randomHealthIncrease);
                    if (registry.healths.has(hero)) {
                        Health& heroHealth = registry.healths.get(hero);
                        heroHealth.max_hp += randomHealthIncrease;
                        heroHealth.hp = heroHealth.max_hp;
                        printf("Increased max health to %d!\n", heroHealth.max_hp);
                    }
                } else {
                    printf("Increasing max mana by %d!\n", randomManaIncrease);
                    if (registry.manas.has(hero)) {
                        Mana& heroMana = registry.manas.get(hero);
                        heroMana.maxMana += randomManaIncrease;
                        heroMana.currentMana = heroMana.maxMana;
                        printf("Increased max mana to %d!\n", heroMana.maxMana);
                    }
                }
            },
            nullptr
        }
    };
}