//
// Created by pesso
//

#pragma once

#include "Actor.h"

class Missile : public Actor
{
public:
    explicit Missile(Game* game, const Vector2& initialPosition, float moveSpeed = 150.0f, float lifetime = 5.0f,
                     const std::string& spritePath = "../Assets/Sprites/Chaser/roc.png", const Vector3& particleColor = Vector3(1.0f, 0.5f, 0.0f));
    const char* GetName() const override { return "Missile"; }

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void Kill() override;

private:
    void UpdateSteering(float deltaTime);
    void EmitFireTrail(float deltaTime);
    
    float mMoveSpeed;
    float mLifetime;
    float mLifeTimer;
    float mParticleEmitTimer;
    float mParticleEmitRate;
    
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mSpriteComponent;
    class ParticleSystemComponent* mFireTrail;
};
