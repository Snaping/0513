#pragma once
#include "Character.h"
#include "../engine/Input.h"

class Player : public Character {
public:
    Player();
    ~Player() override;

    void SetInput(Input* input);
    Input* GetInput() const;

    void SetControlKeys(int leftKey, int rightKey, int jumpKey, int punchKey, int kickKey, int blockKey);

    void Update(float deltaTime) override;

protected:
    virtual void HandleInput();

    Input* m_input;

    int m_leftKey;
    int m_rightKey;
    int m_jumpKey;
    int m_punchKey;
    int m_kickKey;
    int m_blockKey;
};
