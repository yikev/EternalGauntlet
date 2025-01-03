#pragma once

#include "tiny_ecs.hpp"

class WorldSystem;

class DecisionTree {
public:
    virtual ~DecisionTree() = default;
    virtual void execute(WorldSystem& world, Entity enemy, Entity player) = 0;
};

class HealerDecisionTree : public DecisionTree {
public:
    void execute(WorldSystem& world, Entity healer, Entity player) override;
};

class DamageDealerDecisionTree : public DecisionTree {
public:
    void execute(WorldSystem& world, Entity attacker, Entity player) override;
};