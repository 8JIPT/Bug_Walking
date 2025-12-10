#pragma once
#include "Actor.h"
class RigidBodyComponent;
class AABBColliderComponent;
class AnimatorComponent;

class BossBullet : public Actor {
public:
    explicit BossBullet(class Game* game, float direction, float speedY = 0.0f);
    const char* GetName() const override { return "BossBullet"; }
    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
private:
    float mSpeed;
    float mSpeedY;
    float mLiveTime;
    class RigidBodyComponent* mRigidBody;
    class AABBColliderComponent* mCollider;
    class AnimatorComponent* mAnimator;
};
