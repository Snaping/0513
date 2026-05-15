#pragma once
#include "../engine/Vector2.h"
#include "../engine/Physics.h"
#include "../engine/Collision.h"
#include "../engine/Sprite.h"
#include "../engine/Color.h"

enum class CharacterState {
    Idle,
    Walking,
    Jumping,
    Falling,
    Punching,
    Kicking,
    Blocking,
    Hurt,
    Dead
};

enum class FacingDirection {
    Left,
    Right
};

class Character {
public:
    Character();
    virtual ~Character();

    virtual void Update(float deltaTime);
    virtual void Draw(Renderer* renderer);

    void SetPosition(const Vector2& position);
    Vector2 GetPosition() const;

    void SetVelocity(const Vector2& velocity);
    Vector2 GetVelocity() const;

    void SetFacing(FacingDirection direction);
    FacingDirection GetFacing() const;

    void SetState(CharacterState state);
    CharacterState GetState() const;

    void SetMaxHealth(float health);
    float GetMaxHealth() const;
    float GetCurrentHealth() const;
    void TakeDamage(float damage);
    bool IsDead() const;

    void SetMaxStamina(float stamina);
    float GetMaxStamina() const;
    float GetCurrentStamina() const;
    void ConsumeStamina(float amount);
    void RecoverStamina(float amount);

    void SetMoveSpeed(float speed);
    float GetMoveSpeed() const;

    void SetJumpForce(float force);
    float GetJumpForce() const;

    void SetAttackDamage(float damage);
    float GetAttackDamage() const;

    void SetAttackRange(float range);
    float GetAttackRange() const;

    void SetAttackCooldown(float cooldown);
    float GetAttackCooldown() const;

    Collider* GetBodyCollider();
    Collider* GetAttackCollider();

    PhysicsBody* GetPhysicsBody();

    void MoveLeft();
    void MoveRight();
    void StopMoving();
    void Jump();
    void Punch();
    void Kick();
    void Block();
    void StopBlocking();

    bool IsGrounded() const;
    void SetGrounded(bool grounded);

    bool IsAttacking() const;
    bool IsBlocking() const;

    void OnAttackHit(Character* target);

    void SetColor(const Color& color);
    Color GetColor() const;

    void SetSize(const Vector2& size);
    Vector2 GetSize() const;

protected:
    virtual void UpdatePhysics(float deltaTime);
    virtual void UpdateState(float deltaTime);
    virtual void UpdateAnimations();
    virtual void UpdateAttackTimers(float deltaTime);
    virtual void UpdateStaminaRecovery(float deltaTime);

    virtual void CheckAttackCollision();

    CharacterState m_state;
    FacingDirection m_facing;

    float m_maxHealth;
    float m_currentHealth;
    float m_maxStamina;
    float m_currentStamina;

    float m_moveSpeed;
    float m_jumpForce;
    float m_attackDamage;
    float m_attackRange;
    float m_attackCooldown;
    float m_attackTimer;
    float m_hurtTimer;

    bool m_grounded;
    bool m_isAttacking;
    bool m_isBlocking;
    bool m_hasHitTarget;

    PhysicsBody m_physicsBody;
    Collider m_bodyCollider;
    Collider m_attackCollider;

    BoxSprite* m_bodySprite;
    BoxSprite* m_headSprite;
    BoxSprite* m_armSprite;
    BoxSprite* m_legSprite;

    Color m_color;
    Vector2 m_size;

    float m_moveDirection;
};
