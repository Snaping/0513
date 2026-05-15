#include "AIPlayer.h"
#include <cstdlib>
#include <cmath>

AIPlayer::AIPlayer()
    : Character(),
      m_target(nullptr),
      m_aiState(AIState::Idle),
      m_difficulty(2),
      m_decisionTimer(0.0f),
      m_decisionInterval(0.3f),
      m_attackChance(0.4f),
      m_blockChance(0.3f),
      m_jumpChance(0.2f),
      m_approachChance(0.5f),
      m_retreatChance(0.2f)
{
    SetColor(Color::Red());
}

AIPlayer::~AIPlayer() {
}

void AIPlayer::SetTarget(Character* target) {
    m_target = target;
}

Character* AIPlayer::GetTarget() const {
    return m_target;
}

void AIPlayer::SetDifficulty(int level) {
    m_difficulty = level;
    switch (level) {
        case 1:
            m_decisionInterval = 0.5f;
            m_attackChance = 0.2f;
            m_blockChance = 0.1f;
            break;
        case 2:
            m_decisionInterval = 0.3f;
            m_attackChance = 0.4f;
            m_blockChance = 0.3f;
            break;
        case 3:
            m_decisionInterval = 0.15f;
            m_attackChance = 0.6f;
            m_blockChance = 0.5f;
            break;
    }
}

int AIPlayer::GetDifficulty() const {
    return m_difficulty;
}

void AIPlayer::Update(float deltaTime) {
    if (!IsDead()) {
        UpdateAI(deltaTime);
    }
    Character::Update(deltaTime);
}

void AIPlayer::UpdateAI(float deltaTime) {
    if (!m_target) return;

    if (GetFacing() == FacingDirection::Right && m_target->GetPosition().x < GetPosition().x) {
        SetFacing(FacingDirection::Left);
    } else if (GetFacing() == FacingDirection::Left && m_target->GetPosition().x > GetPosition().x) {
        SetFacing(FacingDirection::Right);
    }

    m_decisionTimer -= deltaTime;
    if (m_decisionTimer <= 0) {
        m_decisionTimer = m_decisionInterval;
        MakeDecision();
    }
}

void AIPlayer::MakeDecision() {
    if (!m_target || IsAttacking() || GetState() == CharacterState::Hurt) return;

    float distance = GetDistanceToTarget();
    bool inRange = IsTargetInAttackRange();
    bool targetAttacking = IsTargetAttacking();

    if (targetAttacking) {
        float blockRand = (float)rand() / RAND_MAX;
        if (blockRand < m_blockChance) {
            ExecuteAction(AIState::Block);
            return;
        }
    }

    if (inRange) {
        float attackRand = (float)rand() / RAND_MAX;
        if (attackRand < m_attackChance) {
            ExecuteAction(AIState::Attack);
            return;
        }

        float retreatRand = (float)rand() / RAND_MAX;
        if (retreatRand < m_retreatChance) {
            ExecuteAction(AIState::Retreat);
            return;
        }
    } else if (distance < 300) {
        float approachRand = (float)rand() / RAND_MAX;
        if (approachRand < m_approachChance) {
            ExecuteAction(AIState::Approach);
            return;
        }
    } else {
        ExecuteAction(AIState::Approach);
        return;
    }

    float jumpRand = (float)rand() / RAND_MAX;
    if (jumpRand < m_jumpChance) {
        ExecuteAction(AIState::Jump);
        return;
    }

    ExecuteAction(AIState::Idle);
}

void AIPlayer::ExecuteAction(AIState action) {
    m_aiState = action;
    StopBlocking();
    StopMoving();

    switch (action) {
        case AIState::Idle:
            break;
        case AIState::Approach:
            if (m_target) {
                if (m_target->GetPosition().x < GetPosition().x) {
                    MoveLeft();
                } else {
                    MoveRight();
                }
            }
            break;
        case AIState::Retreat:
            if (m_target) {
                if (m_target->GetPosition().x < GetPosition().x) {
                    MoveRight();
                } else {
                    MoveLeft();
                }
            }
            break;
        case AIState::Attack: {
            float attackRand = (float)rand() / RAND_MAX;
            if (attackRand < 0.5f) {
                Punch();
            } else {
                Kick();
            }
            break;
        }
        case AIState::Block:
            Block();
            break;
        case AIState::Jump:
            Jump();
            break;
    }
}

float AIPlayer::GetDistanceToTarget() const {
    if (!m_target) return 1000.0f;
    return fabsf(GetPosition().x - m_target->GetPosition().x);
}

bool AIPlayer::IsTargetInAttackRange() const {
    return GetDistanceToTarget() < GetAttackRange() + 30;
}

bool AIPlayer::IsTargetAttacking() const {
    if (!m_target) return false;
    return m_target->IsAttacking();
}
