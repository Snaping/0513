#pragma once
#include "Character.h"

enum class AIState {
    Idle,
    Approach,
    Retreat,
    Attack,
    Block,
    Jump
};

class AIPlayer : public Character {
public:
    AIPlayer();
    ~AIPlayer() override;

    void SetTarget(Character* target);
    Character* GetTarget() const;

    void SetDifficulty(int level);
    int GetDifficulty() const;

    void Update(float deltaTime) override;

protected:
    virtual void UpdateAI(float deltaTime);
    virtual void MakeDecision();
    virtual void ExecuteAction(AIState action);

    float GetDistanceToTarget() const;
    bool IsTargetInAttackRange() const;
    bool IsTargetAttacking() const;

    Character* m_target;
    AIState m_aiState;
    int m_difficulty;

    float m_decisionTimer;
    float m_decisionInterval;

    float m_attackChance;
    float m_blockChance;
    float m_jumpChance;
    float m_approachChance;
    float m_retreatChance;
};
