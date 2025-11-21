//
// Created by pesso on 24/10/2025.
//

#pragma once
#include "Actor.h"

class Mushroom : public Actor {
public:
    explicit Mushroom(Game* game, const Vector2& startPos, float speed = 150.0f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

private:
    float mSpeed;
    class RigidBodyComponent* mRigidBody;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mCollider;
};
