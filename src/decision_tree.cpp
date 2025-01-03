#include "decision_tree.hpp"
#include "world_system.hpp"
#include "tiny_ecs.hpp"
#include "tiny_ecs_registry.hpp"
#include "enemy_attacks.hpp"
#include <algorithm>

void HealerDecisionTree::execute(WorldSystem& world, Entity healer, Entity player) {
     std::vector<Attack> availableAttacks = getAvailableAttacksForEnemyType(registry.enemyTypes.get(healer).type);

    // Get healing attack from available attacks
    Attack healingAttack = world.getHealingAttack(availableAttacks);

    // Check if enemies can kill the player
    if (world.canAttackKillPlayer(healer, player)) {
        int bestAttackIndex = world.getBestAttackIndex(availableAttacks);
        world.enemyAttack(bestAttackIndex, healer);
        return; // Action executed, exit
    }

    // Find the lowest HP ally to heal
    Entity targetToHeal = world.findLowestHpAlly(healer);
    Health& targetHealth = registry.healths.get(targetToHeal);

    if (targetToHeal != healer && healingAttack.heal > 0 && targetHealth.hp <= (targetHealth.max_hp / 2)) {
        world.applyHeal(healer, targetToHeal, healingAttack);
    } else {
        // attack if no one to heal
        int bestAttackIndex = world.getBestAttackIndex(availableAttacks);
        world.enemyAttack(bestAttackIndex, healer);
    }
}

void DamageDealerDecisionTree::execute(WorldSystem& world, Entity attacker, Entity player) {
    std::vector<Attack> availableAttacks = getAvailableAttacksForEnemyType(registry.enemyTypes.get(attacker).type);

    // Check if enemies can kill player
    if (world.canAttackKillPlayer(attacker, player)) {
        int bestAttackIndex = world.getBestAttackIndex(availableAttacks);
        world.enemyAttack(bestAttackIndex, attacker);
    } else {
        // Use the most damaging attack available
        int bestAttackIndex = world.getBestAttackIndex(availableAttacks);
        world.enemyAttack(bestAttackIndex, attacker);
    }
}