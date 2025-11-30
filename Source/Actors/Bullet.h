#pragma once
#include "Actor.h"

class Bullet : public Actor {
public:
    Bullet(class Game* game, float direction);

    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;

private:
    class AABBColliderComponent* mCollider;
    class RectComponent* mRect;
    class RigidBodyComponent* mRigidBody;
    float mSpeed;
    float mLiveTime;
};