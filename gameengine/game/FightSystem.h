#pragma once
#include "Character.h"
#include "Player.h"
#include "AIPlayer.h"
#include "../engine/Physics.h"
#include <vector>

enum class FightState {
    Ready,
    Fighting,
    RoundEnd,
    GameEnd
};

class FightSystem {
public:
    FightSystem();
    ~FightSystem();

    void Initialize(int arenaWidth, int arenaHeight);
    void Update(float deltaTime);
    void Draw(Renderer* renderer);

    Player* GetPlayer1() const;
    AIPlayer* GetPlayer2() const;

    void SetRoundTime(float time);
    float GetRoundTime() const;
    float GetCurrentTime() const;

    void StartFight();
    void PauseFight();
    void ResumeFight();
    void ResetFight();

    FightState GetState() const;

    Character* GetWinner() const;

    void SetArenaBounds(float left, float right, float groundY);
    void CheckArenaBounds(Character* character);

    void CheckAttackCollisions();

    int GetPlayer1Wins() const;
    int GetPlayer2Wins() const;

    void SetWinRounds(int rounds);
    int GetWinRounds() const;

protected:
    void UpdateRoundTimer(float deltaTime);
    void CheckKnockout();
    void CheckRoundEnd();
    void CheckGameEnd();
    void EndRound(Character* winner);
    void EndGame(Character* winner);

    Player* m_player1;
    AIPlayer* m_player2;
    Character* m_winner;

    FightState m_state;

    float m_roundTime;
    float m_currentTime;

    int m_player1Wins;
    int m_player2Wins;
    int m_winRounds;

    float m_arenaLeft;
    float m_arenaRight;
    float m_arenaGroundY;
    int m_arenaWidth;
    int m_arenaHeight;

    bool m_paused;
    float m_readyTimer;
};
