//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "Actor.h"

enum class RepairLevel {
    Critical = 1,
    Damaged = 2,
    Fixed = 3
};

class Mario : public Actor
{
public:
    explicit Mario(Game* game, float forwardSpeed = 1500.0f, float jumpSpeed = -750.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;

    void PowerUp();
    void PowerDown();

    bool IsDead(){ return mIsDead;}

    void SetRepairLevel(RepairLevel level);

private:
    void ManageAnimations();
    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDead;

    bool mIsSuper = false;
    bool mHitThisFrame = false;
    float mHitTimer = 0.0f;
    const float HIT_COOLDOWN = 0.5f;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mNormalDraw;
    class AnimatorComponent* mSuperDraw;
    class AABBColliderComponent* mColliderComponent;
    class ParticleSystemComponent* mFireBalls;

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
};