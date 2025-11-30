//
// Created by pesso
//

#pragma once

#include "Actor.h"

enum class ChaserMode
{
    Following,
    Battle,         //Player stopped, enters battle mode
    Cooldown        //Player moved, waiting before following again
};

class Chaser : public Actor
{
public:
    explicit Chaser(Game* game, float moveSpeed = 100.0f, int maxHitPoints = 3);
    const char* GetName() const override { return "Chaser"; }

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;
    void TakeDamage(int damage = 1) override;

    int GetHitPoints() const { return mHitPoints; }

private:
    void UpdateSteering(float deltaTime);
    void UpdateBattle(float deltaTime);
    void UpdateCooldown(float deltaTime);
    Vector2 CalculateTargetPosition();
    bool HasPlayerMoved();

    int mHitPoints;
    int mMaxHitPoints;
    float mMoveSpeed;
    float mDyingTimer;
    bool mIsDying;
    float mDamageTimer;
    std::string mPreviousAnimation;

    // Mode system
    ChaserMode mMode;
    float mCooldownTimer;
    static constexpr float COOLDOWN_TIME = 2.0f;
    
    // Battle mode
    float mMissileFireTimer;
    float mMissileFireRate;
    float mBattleMissileFireRate;
    
    // Steering behavior parameters
    float mOffsetDistance;
    float mRandomYOffset;
    int mOffsetSide;
    Vector2 mTargetPosition;
    Vector2 mLastPlayerPosition;

    // Components
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};
