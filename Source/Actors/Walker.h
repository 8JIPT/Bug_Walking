//
// Created by pesso
//

#pragma once

#include "Actor.h"

enum class WalkerMode
{
    Idle,       // Patrolling left and right
    Following,  // Moving toward player
    Battle      // Close to player, in attack stance
};

class Walker : public Actor
{
public:
    explicit Walker(Game* game, const Vector2& spawnPosition, float moveSpeed = 80.0f, int maxHitPoints = 2);
    const char* GetName() const override { return "Walker"; }

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;
    void TakeDamage(int damage = 1) override;

    int GetHitPoints() const { return mHitPoints; }

private:
    void UpdateIdle(float deltaTime);
    void UpdateFollowing(float deltaTime);
    void UpdateBattle(float deltaTime);
    bool CanSeePlayer();
    
    int mHitPoints;
    int mMaxHitPoints;
    float mMoveSpeed;
    float mDyingTimer;
    bool mIsDying;
    float mDamageTimer;
    bool mTakingDamage;

    // Mode system
    WalkerMode mMode;
    
    // Idle patrol behavior
    Vector2 mSpawnPosition;
    float mPatrolDistance;
    float mPauseTimer;
    float mPauseTime;
    bool mMovingRight;
    
    // Following behavior
    static constexpr float DETECTION_RANGE = 400.0f;
    static constexpr float ATTACK_DISTX = 80.0f;
    static constexpr float ATTACK_DISTY = 288.0f;
    
    // Components
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};
