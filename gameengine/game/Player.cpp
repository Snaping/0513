#include "Player.h"

Player::Player()
    : Character(),
      m_input(nullptr),
      m_leftKey(Input::KEY_A),
      m_rightKey(Input::KEY_D),
      m_jumpKey(Input::KEY_W),
      m_punchKey(Input::KEY_J),
      m_kickKey(Input::KEY_K),
      m_blockKey(Input::KEY_S)
{
}

Player::~Player() {
}

void Player::SetInput(Input* input) {
    m_input = input;
}

Input* Player::GetInput() const {
    return m_input;
}

void Player::SetControlKeys(int leftKey, int rightKey, int jumpKey, int punchKey, int kickKey, int blockKey) {
    m_leftKey = leftKey;
    m_rightKey = rightKey;
    m_jumpKey = jumpKey;
    m_punchKey = punchKey;
    m_kickKey = kickKey;
    m_blockKey = blockKey;
}

void Player::Update(float deltaTime) {
    if (!IsDead()) {
        HandleInput();
    }
    Character::Update(deltaTime);
}

void Player::HandleInput() {
    if (!m_input) return;

    bool moving = false;

    if (m_input->IsKeyDown(m_leftKey)) {
        MoveLeft();
        moving = true;
    }

    if (m_input->IsKeyDown(m_rightKey)) {
        MoveRight();
        moving = true;
    }

    if (!moving && !IsAttacking() && !IsBlocking() && GetState() != CharacterState::Hurt) {
        StopMoving();
    }

    if (m_input->IsKeyPressed(m_jumpKey)) {
        Jump();
    }

    if (m_input->IsKeyPressed(m_punchKey)) {
        Punch();
    }

    if (m_input->IsKeyPressed(m_kickKey)) {
        Kick();
    }

    if (m_input->IsKeyDown(m_blockKey)) {
        Block();
    } else {
        StopBlocking();
    }
}
