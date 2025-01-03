#pragma once

#include <vector>
#include <string>
#include "common.hpp" 

struct EnemyData {
    std::string type;
    vec2 position;
};

struct CombatStage {
    std::vector<EnemyData> enemies; 
};

// Updated declaration to match the definition
const std::vector<CombatStage>& getCombatStages();