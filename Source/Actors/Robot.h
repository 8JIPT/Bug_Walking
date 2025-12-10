// Robot.h
#pragma once
#include "Actor.h"
#include "RepairLevel.h"

class Robot : public Actor
{
public:
    explicit Robot(Game* game, float forwardSpeed = 1200.0f, float jumpSpeed = -900.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;
    bool IsDead() { return mIsDead; }

    void SetRepairLevel(RepairLevel level);
    void TakeDamage(int damage = 1) override;
    int GetHitPoints() const { return mHitPoints; }
    int GetMaxHitPoints() const { return mMaxHitPoints; }
    RepairLevel GetRepairLevel() const { return mCurrentLevel; }

    void UpgradeRepairLevel();

private:
    void ManageAnimations();

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDead;

    float mJumpCutoff;

    // Glitch / damage
    bool mHitThisFrame = false;
    float mHitTimer = 0.0f; // used for invincibility frames (seconds)
    const float HIT_COOLDOWN = 1.5f; // invincibility duration after taking damage

    int mHitPoints = 5;
    int mMaxHitPoints = 5;

    float mFireBallCooldown = 0.3f;

    void HandleShooting();
    void HandleGlitches(float deltaTime);

    RepairLevel mCurrentLevel;

    float mShootCooldown;
    float mShootCooldownTimer;

    float mTimeBetweenGlitches;
    float mShootFailChance;

    float mGlitchTimer;
    float mGlitchDurationTimer;
    bool mIsInputLocked;
    
    // Stomp immunity - prevents damage from stomped enemy until Robot exits collision
    class Actor* mStompedEnemy = nullptr;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mNormalDraw;
    class AABBColliderComponent* mColliderComponent;
};
