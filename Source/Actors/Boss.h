//
// Criado por Edmundo
//

#pragma once

#include "Actor.h"

enum class BossState{
    Patrol,         // Patrulhando
    Shooting        // Atirando
};

class Boss : public Actor{

public:
    explicit Boss(Game* game, const Vector2& spawnPosition, float moveSpeed = 80.0f, int maxHitPoints = 10);
    const char* GetName() const override { return "Boss"; }

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnProcessInput(const Uint8* keyState) override;

    void Kill() override;
    void TakeDamage(int damage = 1) override;

    int GetHitPoints() const { return mHitPoints; }

private:
    void Shoot();
    void LaunchMissile();
    bool CanSeePlayer();

    // Propriedades do boss
    int mHitPoints;
    int mMaxHitPoints;
    float mMoveSpeed;
    float mDyingTimer;
    bool mIsDying;

    // Sistema de estados
    BossState mState;

    // Posição inicial para patrulha
    Vector2 mSpawnPosition;
    float mPatrolDistance;
    int mPatrolDirection;  // 1 = direita, -1 = esquerda

    // Detecção e tiro
    static constexpr float DETECTION_RANGE = 500.0f;
    float mShootTimer;
    float mShootCooldown;
    float mMissileTimer;
    float mMissileCooldown;

    // Componentes
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mAnimatorComponent;
};