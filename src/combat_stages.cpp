#include <vector>
#include "combat_stages.hpp"

const std::vector<CombatStage>& getCombatStages() {
    static std::vector<CombatStage> combatStages;

    if (combatStages.empty()) {

        CombatStage stage1;
        stage1.enemies = {
            {"Melee", {600, 0}},
            {"Healer", {200, 0}}
        };
        combatStages.push_back(stage1);

        CombatStage stage2;
        stage2.enemies = {
            {"Melee", {1000, 0}},
            {"Ranged", {200, 0}},
            {"Ranged", {600, 0}}
        };
        combatStages.push_back(stage2);

        CombatStage stage3;
        stage3.enemies = {
            {"Melee", {600, 0}}
        };
        combatStages.push_back(stage3);

        CombatStage stage4;
        stage4.enemies = {
            {"Boss", {600, 0}}
        };
        combatStages.push_back(stage4);

        CombatStage stage5;
        stage5.enemies = {
            {"Tank", {1000, 0}},
            {"Melee", {200, 0}},
            {"Ranged", {600, 0}}
        };
        combatStages.push_back(stage5);

        CombatStage stage6;
        stage6.enemies = {
            {"Tank", {1000, 0}},
            {"Tank", {200, 0}},
            {"Ranged", {600, 0}}
        };
        combatStages.push_back(stage6);
    }

    return combatStages;
}