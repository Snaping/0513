#define NOMINMAX
#include "Character.h"
#include "../engine/Renderer.h"
#include <algorithm>

Character::Character()
    : m_state(CharacterState::Idle),
      m_facing(FacingDirection::Right),
      m_maxHealth(100.0f),
      m_currentHealth(100.0f),
      m_maxStamina(100.0f),
      m_currentStamina(100.0f),
      m_moveSpeed(300.0f),
      m_jumpForce(600.0f),
      m_attackDamage(10.0f),
      m_attackRange(80.0f),
      m_attackCooldown(0.5f),
      m_attackTimer(0.0f),
      m_hurtTimer(0.0f),
      m_grounded(false),
      m_isAttacking(false),
      m_isBlocking(false),
      m_hasHitTarget(false),
      m_color(Color::Blue()),
      m_size(60.0f, 120.0f),
      m_moveDirection(0.0f)
{
    m_bodySprite = new BoxSprite();
    m_headSprite = new BoxSprite();
    m_armSprite = new BoxSprite();
    m_legSprite = new BoxSprite();

    m_bodyCollider.SetBounds(Rect(0, 0, m_size.x, m_size.y));
    m_attackCollider.SetBounds(Rect(0, 0, m_attackRange, 40.0f));

    m_physicsBody.SetUseGravity(true);
    m_physicsBody.SetGravityScale(1.0f);
}

Character::~Character() {
    delete m_bodySprite;
    delete m_headSprite;
    delete m_armSprite;
    delete m_legSprite;
}

void Character::Update(float deltaTime) {
    UpdatePhysics(deltaTime);
    UpdateState(deltaTime);
    UpdateAttackTimers(deltaTime);
    UpdateStaminaRecovery(deltaTime);
}

void Character::Draw(Renderer* renderer) {
    Vector2 pos = GetPosition();

    m_bodySprite->SetPosition(pos);
    m_bodySprite->SetSize(Vector2(m_size.x, m_size.y * 0.6f));
    m_bodySprite->SetColor(m_color);
    m_bodySprite->Draw(renderer);

    m_headSprite->SetPosition(Vector2(pos.x, pos.y - m_size.y * 0.35f));
    m_headSprite->SetSize(Vector2(m_size.x * 0.8f, m_size.y * 0.35f));
    m_headSprite->SetColor(m_color);
    m_headSprite->Draw(renderer);

    if (m_isAttacking) {
        float armOffsetX = (m_facing == FacingDirection::Right) ? m_size.x * 0.5f : -m_size.x * 0.5f;
        float attackOffset = (m_facing == FacingDirection::Right) ? 30.0f : -30.0f;
        m_armSprite->SetPosition(Vector2(pos.x + armOffsetX + attackOffset, pos.y - m_size.y * 0.2f));
        m_armSprite->SetSize(Vector2(m_attackRange * 0.5f, 20.0f));
        m_armSprite->SetColor(Color::Yellow());
        m_armSprite->Draw(renderer);
    }

    if (m_state == CharacterState::Blocking) {
        float blockOffset = (m_facing == FacingDirection::Right) ? -m_size.x * 0.6f : m_size.x * 0.4f;
        Rect blockRect(pos.x + blockOffset, pos.y - m_size.y * 0.5f, 20.0f, m_size.y * 0.6f);
        renderer->FillRect(blockRect, Color::Cyan());
    }

    if (m_state == CharacterState::Hurt && ((int)(m_hurtTimer * 10) % 2 == 0)) {
        Rect hurtRect = m_bodyCollider.GetBounds();
        hurtRect.x -= 5;
        hurtRect.y -= 5;
        hurtRect.width += 10;
        hurtRect.height += 10;
        renderer->FillRect(hurtRect, Color(255, 255, 255, 100));
    }
}

void Character::SetPosition(const Vector2& position) {
    m_physicsBody.SetPosition(position);
    m_bodyCollider.SetPosition(Vector2(position.x - m_size.x * 0.5f, position.y - m_size.y));
}

Vector2 Character::GetPosition() const {
    return m_physicsBody.GetPosition();
}

void Character::SetVelocity(const Vector2& velocity) {
    m_physicsBody.SetVelocity(velocity);
}

Vector2 Character::GetVelocity() const {
    return m_physicsBody.GetVelocity();
}

void Character::SetFacing(FacingDirection direction) {
    m_facing = direction;
}

FacingDirection Character::GetFacing() const {
    return m_facing;
}

void Character::SetState(CharacterState state) {
    if (m_state != CharacterState::Dead) {
        m_state = state;
    }
}

CharacterState Character::GetState() const {
    return m_state;
}

void Character::SetMaxHealth(float health) {
    m_maxHealth = health;
    m_currentHealth = std::min(m_currentHealth, m_maxHealth);
}

float Character::GetMaxHealth() const {
    return m_maxHealth;
}

float Character::GetCurrentHealth() const {
    return m_currentHealth;
}

void Character::TakeDamage(float damage) {
    if (m_isBlocking) {
        damage *= 0.3f;
        ConsumeStamina(10.0f);
    }

    m_currentHealth -= damage;
    m_hurtTimer = 0.3f;

    if (m_currentHealth <= 0) {
        m_currentHealth = 0;
        m_state = CharacterState::Dead;
    } else if (m_state != CharacterState::Dead) {
        m_state = CharacterState::Hurt;
    }
}

bool Character::IsDead() const {
    return m_state == CharacterState::Dead;
}

void Character::SetMaxStamina(float stamina) {
    m_maxStamina = stamina;
    m_currentStamina = std::min(m_currentStamina, m_maxStamina);
}

float Character::GetMaxStamina() const {
    return m_maxStamina;
}

float Character::GetCurrentStamina() const {
    return m_currentStamina;
}

void Character::ConsumeStamina(float amount) {
    m_currentStamina = std::max(0.0f, m_currentStamina - amount);
}

void Character::RecoverStamina(float amount) {
    m_currentStamina = std::min(m_maxStamina, m_currentStamina + amount);
}

void Character::SetMoveSpeed(float speed) {
    m_moveSpeed = speed;
}

float Character::GetMoveSpeed() const {
    return m_moveSpeed;
}

void Character::SetJumpForce(float force) {
    m_jumpForce = force;
}

float Character::GetJumpForce() const {
    return m_jumpForce;
}

void Character::SetAttackDamage(float damage) {
    m_attackDamage = damage;
}

float Character::GetAttackDamage() const {
    return m_attackDamage;
}

void Character::SetAttackRange(float range) {
    m_attackRange = range;
}

float Character::GetAttackRange() const {
    return m_attackRange;
}

void Character::SetAttackCooldown(float cooldown) {
    m_attackCooldown = cooldown;
}

float Character::GetAttackCooldown() const {
    return m_attackCooldown;
}

Collider* Character::GetBodyCollider() {
    return &m_bodyCollider;
}

Collider* Character::GetAttackCollider() {
    return &m_attackCollider;
}

PhysicsBody* Character::GetPhysicsBody() {
    return &m_physicsBody;
}

void Character::MoveLeft() {
    if (m_state != CharacterState::Dead && !m_isAttacking && m_state != CharacterState::Hurt) {
        m_moveDirection = -1.0f;
        m_facing = FacingDirection::Left;
        if (m_grounded) {
            m_state = CharacterState::Walking;
        }
    }
}

void Character::MoveRight() {
    if (m_state != CharacterState::Dead && !m_isAttacking && m_state != CharacterState::Hurt) {
        m_moveDirection = 1.0f;
        m_facing = FacingDirection::Right;
        if (m_grounded) {
            m_state = CharacterState::Walking;
        }
    }
}

void Character::StopMoving() {
    m_moveDirection = 0.0f;
    if (m_grounded && !m_isAttacking && m_state != CharacterState::Hurt && !m_isBlocking) {
        m_state = CharacterState::Idle;
    }
}

void Character::Jump() {
    if (m_grounded && m_state != CharacterState::Dead && !m_isAttacking && m_state != CharacterState::Hurt) {
        m_physicsBody.SetVelocity(Vector2(m_physicsBody.GetVelocity().x, -m_jumpForce));
        m_grounded = false;
        m_state = CharacterState::Jumping;
    }
}

void Character::Punch() {
    if (m_state != CharacterState::Dead && m_attackTimer <= 0 && !m_isAttacking && m_state != CharacterState::Hurt) {
        if (m_currentStamina >= 10.0f) {
            m_state = CharacterState::Punching;
            m_isAttacking = true;
            m_hasHitTarget = false;
            m_attackTimer = m_attackCooldown;
            ConsumeStamina(10.0f);

            Vector2 pos = GetPosition();
            float attackX = (m_facing == FacingDirection::Right) ? pos.x : pos.x - m_attackRange;
            m_attackCollider.SetPosition(Vector2(attackX, pos.y - m_size.y * 0.4f));
        }
    }
}

void Character::Kick() {
    if (m_state != CharacterState::Dead && m_attackTimer <= 0 && !m_isAttacking && m_state != CharacterState::Hurt) {
        if (m_currentStamina >= 15.0f) {
            m_state = CharacterState::Kicking;
            m_isAttacking = true;
            m_hasHitTarget = false;
            m_attackTimer = m_attackCooldown * 1.5f;
            ConsumeStamina(15.0f);

            Vector2 pos = GetPosition();
            float attackX = (m_facing == FacingDirection::Right) ? pos.x : pos.x - m_attackRange * 1.2f;
            m_attackCollider.SetPosition(Vector2(attackX, pos.y - m_size.y * 0.1f));
            m_attackCollider.SetSize(Vector2(m_attackRange * 1.2f, 50.0f));
        }
    }
}

void Character::Block() {
    if (m_state != CharacterState::Dead && !m_isAttacking && m_state != CharacterState::Hurt) {
        if (m_currentStamina > 0) {
            m_isBlocking = true;
            m_state = CharacterState::Blocking;
            m_moveDirection = 0.0f;
        }
    }
}

void Character::StopBlocking() {
    m_isBlocking = false;
    if (m_state == CharacterState::Blocking) {
        m_state = CharacterState::Idle;
    }
}

bool Character::IsGrounded() const {
    return m_grounded;
}

void Character::SetGrounded(bool grounded) {
    m_grounded = grounded;
    if (grounded && (m_state == CharacterState::Jumping || m_state == CharacterState::Falling)) {
        m_state = CharacterState::Idle;
    }
}

bool Character::IsAttacking() const {
    return m_isAttacking;
}

bool Character::IsBlocking() const {
    return m_isBlocking;
}

void Character::OnAttackHit(Character* target) {
    if (!m_hasHitTarget && target) {
        float damage = m_attackDamage;
        if (m_state == CharacterState::Kicking) {
            damage *= 1.5f;
        }
        target->TakeDamage(damage);
        m_hasHitTarget = true;
    }
}

void Character::SetColor(const Color& color) {
    m_color = color;
}

Color Character::GetColor() const {
    return m_color;
}

void Character::SetSize(const Vector2& size) {
    m_size = size;
    m_bodyCollider.SetSize(size);
}

Vector2 Character::GetSize() const {
    return m_size;
}

void Character::UpdatePhysics(float deltaTime) {
    Vector2 velocity = m_physicsBody.GetVelocity();
    velocity.x = m_moveDirection * m_moveSpeed;

    if (m_isBlocking) {
        velocity.x *= 0.2f;
    }

    if (!m_grounded) {
        velocity.y += 980.0f * deltaTime;
    }

    m_physicsBody.SetVelocity(velocity);
    m_physicsBody.Update(deltaTime);

    Vector2 pos = m_physicsBody.GetPosition();
    m_bodyCollider.SetPosition(Vector2(pos.x - m_size.x * 0.5f, pos.y - m_size.y));
}

void Character::UpdateState(float deltaTime) {
    m_hurtTimer -= deltaTime;
    if (m_hurtTimer <= 0 && m_state == CharacterState::Hurt) {
        m_state = CharacterState::Idle;
    }

    if (!m_grounded && m_physicsBody.GetVelocity().y > 0) {
        m_state = CharacterState::Falling;
    }
}

void Character::UpdateAnimations() {
}

void Character::UpdateAttackTimers(float deltaTime) {
    if (m_attackTimer > 0) {
        m_attackTimer -= deltaTime;
    }

    if (m_isAttacking) {
        float attackDuration = (m_state == CharacterState::Kicking) ? 0.4f : 0.25f;
        if (m_attackTimer <= m_attackCooldown - attackDuration) {
            m_isAttacking = false;
            m_hasHitTarget = false;
            if (m_state == CharacterState::Punching || m_state == CharacterState::Kicking) {
                m_state = CharacterState::Idle;
            }
        }
    }
}

void Character::UpdateStaminaRecovery(float deltaTime) {
    if (!m_isBlocking && !m_isAttacking) {
        RecoverStamina(20.0f * deltaTime);
    }
}

void Character::CheckAttackCollision() {
}
