#include "BossBullet.h"
#include "Robot.h"
#include "Bullet.h"
#include "Boss.h"
#include "../Game.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

BossBullet::BossBullet(Game* game, float direction, float speedY)
    : Actor(game)
    , mSpeed(900.0f * direction)
    , mSpeedY(speedY)
    , mLiveTime(8.0f)
    , mRigidBody(nullptr)
    , mCollider(nullptr)
    , mAnimator(nullptr)
{
    // Usar o sprite do BossGunShot
    mAnimator = new AnimatorComponent(this, 
        "../Assets/Sprites/BossGunShot/shoot.png",
        "../Assets/Sprites/BossGunShot/shoot.json",
        22, 16, 110);
    
    mAnimator->AddAnimation("shoot", std::vector<int>{0, 1, 2, 3});
    mAnimator->SetAnimFPS(12.0f);
    mAnimator->SetAnimation("shoot");

    mRigidBody = new RigidBodyComponent(this, 1.0f, 0.0f);
    mRigidBody->SetGravityScale(0.0f);
    mRigidBody->SetVelocity(Vector2(mSpeed, mSpeedY));

    // Camada Enemy para que colida com Player
    mCollider = new AABBColliderComponent(this, 0, 0, 11, 8, ColliderLayer::Enemy, false, 10);
}

void BossBullet::OnUpdate(float deltaTime)
{
    // manter velocidade constante
    Vector2 vel = mRigidBody->GetVelocity();
    vel.x = mSpeed;
    vel.y = mSpeedY;
    mRigidBody->SetVelocity(vel);

    // tempo de vida
    mLiveTime -= deltaTime;
    if (mLiveTime <= 0.0f) {
        SetState(ActorState::Destroy);
        return;
    }

    // verificar acerto no Player
    auto& colliders = GetGame()->GetColliders();
    for (auto* otherCollider : colliders) {
        if (otherCollider->GetLayer() == ColliderLayer::Player && otherCollider->IsEnabled()) {
            Vector2 myMin = mCollider->GetMin();
            Vector2 myMax = mCollider->GetMax();
            Vector2 otherMin = otherCollider->GetMin();
            Vector2 otherMax = otherCollider->GetMax();

            bool noOverlap = myMax.x < otherMin.x ||
                             myMin.x > otherMax.x ||
                             myMax.y < otherMin.y ||
                             myMin.y > otherMax.y;

            if (!noOverlap) {
                Actor* player = otherCollider->GetOwner();
                if (player) {
                    player->TakeDamage(1);
                    SDL_Log("BossBullet hit: %s", player->GetName());
                }
                SetState(ActorState::Destroy);
                return;
            }
        }
    }
}

void BossBullet::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Blocks) {
        SetState(ActorState::Destroy);
    }
}
