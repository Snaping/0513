#pragma once
#include <vector>
#include <map>
#include "Sprite.h"

class AnimationFrame {
public:
    AnimationFrame();
    AnimationFrame(Sprite* sprite, float duration);

    void SetSprite(Sprite* sprite);
    Sprite* GetSprite() const;

    void SetDuration(float duration);
    float GetDuration() const;

private:
    Sprite* m_sprite;
    float m_duration;
};

class Animation {
public:
    Animation();

    void AddFrame(Sprite* sprite, float duration);
    void RemoveFrame(int index);
    void ClearFrames();

    int GetFrameCount() const;
    AnimationFrame* GetFrame(int index);

    void SetLoop(bool loop);
    bool IsLooping() const;

    void SetSpeed(float speed);
    float GetSpeed() const;

    void Play();
    void Pause();
    void Stop();
    void Reset();

    bool IsPlaying() const;
    bool IsFinished() const;

    void Update(float deltaTime);
    Sprite* GetCurrentSprite();

private:
    std::vector<AnimationFrame> m_frames;
    int m_currentFrame;
    float m_frameTimer;
    bool m_loop;
    bool m_playing;
    bool m_finished;
    float m_speed;
};

class Animator {
public:
    Animator();

    void AddAnimation(const std::wstring& name, Animation* animation);
    void RemoveAnimation(const std::wstring& name);

    void PlayAnimation(const std::wstring& name);
    void StopAnimation();

    void Update(float deltaTime);
    Sprite* GetCurrentSprite();

    std::wstring GetCurrentAnimationName() const;
    bool IsAnimationFinished() const;

private:
    std::map<std::wstring, Animation*> m_animations;
    Animation* m_currentAnimation;
    std::wstring m_currentAnimationName;
};
