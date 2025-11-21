//
// Created by Lucas N. Ferreira on 30/09/23.
//

#pragma once

#include "Actor.h"

class Goomba : public Actor
{
public:
    explicit Goomba(Game* game, float forwardSpeed = 20.0f, float deathTime = 0.5f);
    const char* GetName() const override { return "Goomba"; }

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;

private:
    bool mIsDying;
    float mForwardSpeed;
    float mDyingTimer;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};