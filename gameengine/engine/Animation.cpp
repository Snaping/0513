#include "Animation.h"
#include <map>

AnimationFrame::AnimationFrame() : m_sprite(nullptr), m_duration(0.1f) {}

AnimationFrame::AnimationFrame(Sprite* sprite, float duration)
    : m_sprite(sprite), m_duration(duration) {}

void AnimationFrame::SetSprite(Sprite* sprite) {
    m_sprite = sprite;
}

Sprite* AnimationFrame::GetSprite() const {
    return m_sprite;
}

void AnimationFrame::SetDuration(float duration) {
    m_duration = duration;
}

float AnimationFrame::GetDuration() const {
    return m_duration;
}

Animation::Animation()
    : m_currentFrame(0), m_frameTimer(0.0f), m_loop(true),
      m_playing(false), m_finished(false), m_speed(1.0f) {}

void Animation::AddFrame(Sprite* sprite, float duration) {
    m_frames.push_back(AnimationFrame(sprite, duration));
}

void Animation::RemoveFrame(int index) {
    if (index >= 0 && index < (int)m_frames.size()) {
        m_frames.erase(m_frames.begin() + index);
    }
}

void Animation::ClearFrames() {
    m_frames.clear();
}

int Animation::GetFrameCount() const {
    return (int)m_frames.size();
}

AnimationFrame* Animation::GetFrame(int index) {
    if (index >= 0 && index < (int)m_frames.size()) {
        return &m_frames[index];
    }
    return nullptr;
}

void Animation::SetLoop(bool loop) {
    m_loop = loop;
}

bool Animation::IsLooping() const {
    return m_loop;
}

void Animation::SetSpeed(float speed) {
    m_speed = speed;
}

float Animation::GetSpeed() const {
    return m_speed;
}

void Animation::Play() {
    m_playing = true;
    m_finished = false;
}

void Animation::Pause() {
    m_playing = false;
}

void Animation::Stop() {
    m_playing = false;
    m_currentFrame = 0;
    m_frameTimer = 0.0f;
    m_finished = true;
}

void Animation::Reset() {
    m_currentFrame = 0;
    m_frameTimer = 0.0f;
    m_finished = false;
}

bool Animation::IsPlaying() const {
    return m_playing;
}

bool Animation::IsFinished() const {
    return m_finished;
}

void Animation::Update(float deltaTime) {
    if (!m_playing || m_frames.empty()) {
        return;
    }

    m_frameTimer += deltaTime * m_speed;

    while (m_frameTimer >= m_frames[m_currentFrame].GetDuration()) {
        m_frameTimer -= m_frames[m_currentFrame].GetDuration();
        m_currentFrame++;

        if (m_currentFrame >= (int)m_frames.size()) {
            if (m_loop) {
                m_currentFrame = 0;
            } else {
                m_currentFrame = (int)m_frames.size() - 1;
                m_playing = false;
                m_finished = true;
                return;
            }
        }
    }
}

Sprite* Animation::GetCurrentSprite() {
    if (!m_frames.empty() && m_currentFrame < (int)m_frames.size()) {
        return m_frames[m_currentFrame].GetSprite();
    }
    return nullptr;
}

Animator::Animator() : m_currentAnimation(nullptr) {}

void Animator::AddAnimation(const std::wstring& name, Animation* animation) {
    m_animations[name] = animation;
}

void Animator::RemoveAnimation(const std::wstring& name) {
    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        m_animations.erase(it);
    }
}

void Animator::PlayAnimation(const std::wstring& name) {
    if (m_currentAnimationName == name && m_currentAnimation && m_currentAnimation->IsPlaying()) {
        return;
    }

    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        if (m_currentAnimation) {
            m_currentAnimation->Stop();
        }
        m_currentAnimation = it->second;
        m_currentAnimationName = name;
        m_currentAnimation->Reset();
        m_currentAnimation->Play();
    }
}

void Animator::StopAnimation() {
    if (m_currentAnimation) {
        m_currentAnimation->Stop();
    }
}

void Animator::Update(float deltaTime) {
    if (m_currentAnimation) {
        m_currentAnimation->Update(deltaTime);
    }
}

Sprite* Animator::GetCurrentSprite() {
    if (m_currentAnimation) {
        return m_currentAnimation->GetCurrentSprite();
    }
    return nullptr;
}

std::wstring Animator::GetCurrentAnimationName() const {
    return m_currentAnimationName;
}

bool Animator::IsAnimationFinished() const {
    if (m_currentAnimation) {
        return m_currentAnimation->IsFinished();
    }
    return true;
}
