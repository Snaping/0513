#include "FightSystem.h"
#include "../engine/Renderer.h"
#include <algorithm>

FightSystem::FightSystem()
    : m_player1(nullptr),
      m_player2(nullptr),
      m_winner(nullptr),
      m_state(FightState::Ready),
      m_roundTime(99.0f),
      m_currentTime(99.0f),
      m_player1Wins(0),
      m_player2Wins(0),
      m_winRounds(2),
      m_arenaLeft(100.0f),
      m_arenaRight(1180.0f),
      m_arenaGroundY(650.0f),
      m_arenaWidth(1280),
      m_arenaHeight(720),
      m_paused(false),
      m_readyTimer(3.0f)
{
}

FightSystem::~FightSystem() {
    delete m_player1;
    delete m_player2;
}

void FightSystem::Initialize(int arenaWidth, int arenaHeight) {
    m_arenaWidth = arenaWidth;
    m_arenaHeight = arenaHeight;
    m_arenaLeft = 100.0f;
    m_arenaRight = (float)arenaWidth - 100.0f;
    m_arenaGroundY = (float)arenaHeight - 70.0f;

    m_player1 = new Player();
    m_player1->SetPosition(Vector2(300.0f, m_arenaGroundY));
    m_player1->SetFacing(FacingDirection::Right);
    m_player1->SetColor(Color::Blue());

    m_player2 = new AIPlayer();
    m_player2->SetPosition(Vector2((float)arenaWidth - 300.0f, m_arenaGroundY));
    m_player2->SetFacing(FacingDirection::Left);
    m_player2->SetTarget(m_player1);
    m_player2->SetColor(Color::Red());
    m_player2->SetDifficulty(2);
}

void FightSystem::Update(float deltaTime) {
    if (m_paused) return;

    switch (m_state) {
        case FightState::Ready:
            m_readyTimer -= deltaTime;
            if (m_readyTimer <= 0) {
                m_state = FightState::Fighting;
            }
            break;
        case FightState::Fighting:
            UpdateRoundTimer(deltaTime);

            m_player1->Update(deltaTime);
            m_player2->Update(deltaTime);

            CheckArenaBounds(m_player1);
            CheckArenaBounds(m_player2);

            CheckAttackCollisions();

            CheckKnockout();
            CheckRoundEnd();
            break;
        case FightState::RoundEnd:
            m_readyTimer -= deltaTime;
            if (m_readyTimer <= 0) {
                CheckGameEnd();
                if (m_state == FightState::RoundEnd) {
                    ResetFight();
                    m_state = FightState::Ready;
                    m_readyTimer = 2.0f;
                }
            }
            break;
        case FightState::GameEnd:
            break;
    }
}

void FightSystem::Draw(Renderer* renderer) {
    Rect ground(0, m_arenaGroundY, (float)m_arenaWidth, 70.0f);
    renderer->FillRect(ground, Color(100, 100, 100));

    Rect background(0, 0, (float)m_arenaWidth, m_arenaGroundY);
    renderer->FillRect(background, Color(50, 80, 120));

    if (m_player1) {
        m_player1->Draw(renderer);
    }
    if (m_player2) {
        m_player2->Draw(renderer);
    }
}

Player* FightSystem::GetPlayer1() const {
    return m_player1;
}

AIPlayer* FightSystem::GetPlayer2() const {
    return m_player2;
}

void FightSystem::SetRoundTime(float time) {
    m_roundTime = time;
}

float FightSystem::GetRoundTime() const {
    return m_roundTime;
}

float FightSystem::GetCurrentTime() const {
    return m_currentTime;
}

void FightSystem::StartFight() {
    m_state = FightState::Fighting;
    m_paused = false;
}

void FightSystem::PauseFight() {
    m_paused = true;
}

void FightSystem::ResumeFight() {
    m_paused = false;
}

void FightSystem::ResetFight() {
    m_currentTime = m_roundTime;

    if (m_player1) {
        m_player1->SetPosition(Vector2(300.0f, m_arenaGroundY));
        m_player1->SetVelocity(Vector2(0, 0));
        m_player1->SetMaxHealth(100.0f);
        m_player1->TakeDamage(-100.0f);
        m_player1->SetMaxStamina(100.0f);
        m_player1->RecoverStamina(100.0f);
        m_player1->SetState(CharacterState::Idle);
        m_player1->SetFacing(FacingDirection::Right);
    }

    if (m_player2) {
        m_player2->SetPosition(Vector2((float)m_arenaWidth - 300.0f, m_arenaGroundY));
        m_player2->SetVelocity(Vector2(0, 0));
        m_player2->SetMaxHealth(100.0f);
        m_player2->TakeDamage(-100.0f);
        m_player2->SetMaxStamina(100.0f);
        m_player2->RecoverStamina(100.0f);
        m_player2->SetState(CharacterState::Idle);
        m_player2->SetFacing(FacingDirection::Left);
    }

    m_winner = nullptr;
}

FightState FightSystem::GetState() const {
    return m_state;
}

Character* FightSystem::GetWinner() const {
    return m_winner;
}

void FightSystem::SetArenaBounds(float left, float right, float groundY) {
    m_arenaLeft = left;
    m_arenaRight = right;
    m_arenaGroundY = groundY;
}

void FightSystem::CheckArenaBounds(Character* character) {
    Vector2 pos = character->GetPosition();
    Vector2 vel = character->GetVelocity();

    if (pos.x - character->GetSize().x * 0.5f < m_arenaLeft) {
        pos.x = m_arenaLeft + character->GetSize().x * 0.5f;
        vel.x = 0;
    }

    if (pos.x + character->GetSize().x * 0.5f > m_arenaRight) {
        pos.x = m_arenaRight - character->GetSize().x * 0.5f;
        vel.x = 0;
    }

    if (pos.y > m_arenaGroundY) {
        pos.y = m_arenaGroundY;
        vel.y = 0;
        character->SetGrounded(true);
    } else {
        character->SetGrounded(false);
    }

    character->SetPosition(pos);
    character->SetVelocity(vel);
}

void FightSystem::CheckAttackCollisions() {
    if (m_player1->IsAttacking() && m_player1->GetAttackCollider()->Intersects(*m_player2->GetBodyCollider())) {
        m_player1->OnAttackHit(m_player2);
    }

    if (m_player2->IsAttacking() && m_player2->GetAttackCollider()->Intersects(*m_player1->GetBodyCollider())) {
        m_player2->OnAttackHit(m_player1);
    }
}

int FightSystem::GetPlayer1Wins() const {
    return m_player1Wins;
}

int FightSystem::GetPlayer2Wins() const {
    return m_player2Wins;
}

void FightSystem::SetWinRounds(int rounds) {
    m_winRounds = rounds;
}

int FightSystem::GetWinRounds() const {
    return m_winRounds;
}

void FightSystem::UpdateRoundTimer(float deltaTime) {
    m_currentTime -= deltaTime;
    if (m_currentTime < 0) {
        m_currentTime = 0;
    }
}

void FightSystem::CheckKnockout() {
    if (m_player1->IsDead() && !m_player2->IsDead()) {
        EndRound(m_player2);
    } else if (!m_player1->IsDead() && m_player2->IsDead()) {
        EndRound(m_player1);
    } else if (m_player1->IsDead() && m_player2->IsDead()) {
        EndRound(nullptr);
    }
}

void FightSystem::CheckRoundEnd() {
    if (m_currentTime <= 0 && m_state == FightState::Fighting) {
        if (m_player1->GetCurrentHealth() > m_player2->GetCurrentHealth()) {
            EndRound(m_player1);
        } else if (m_player2->GetCurrentHealth() > m_player1->GetCurrentHealth()) {
            EndRound(m_player2);
        } else {
            EndRound(nullptr);
        }
    }
}

void FightSystem::CheckGameEnd() {
    if (m_player1Wins >= m_winRounds) {
        EndGame(m_player1);
    } else if (m_player2Wins >= m_winRounds) {
        EndGame(m_player2);
    }
}

void FightSystem::EndRound(Character* winner) {
    m_state = FightState::RoundEnd;
    m_winner = winner;
    m_readyTimer = 3.0f;

    if (winner == m_player1) {
        m_player1Wins++;
    } else if (winner == m_player2) {
        m_player2Wins++;
    }
}

void FightSystem::EndGame(Character* winner) {
    m_state = FightState::GameEnd;
    m_winner = winner;
}
