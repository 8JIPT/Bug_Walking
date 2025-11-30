#include "Bullet.h"
#include "../Game.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Drawing/DrawComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Drawing/RectComponent.h"

Bullet::Bullet(Game* game, float direction)
    : Actor(game)
    , mSpeed(800.0f * direction)
    , mLiveTime(5.0f)
{
    // rect como placeholder
    mRect = new RectComponent(this, 10, 10, static_cast<RendererMode>(0), 110);
    mRect->SetColor(Vector3(255, 255, 0));

    mRigidBody = new RigidBodyComponent(this, 1.0f, 0.0f);
    mRigidBody->SetVelocity(Vector2(mSpeed, 0.0f));

    mCollider = new AABBColliderComponent(this, 0, 0, 10, 10, ColliderLayer::Player, false, 10);

    mRigidBody->SetGravityScale(0.0f);
}

void Bullet::OnUpdate(float deltaTime) {
    Vector2 vel = mRigidBody->GetVelocity();
    vel.x = mSpeed;
    vel.y = 0.0f;
    mRigidBody->SetVelocity(vel);

    mLiveTime -= deltaTime;
    if (mLiveTime <= 0.0f) {
        SetState(ActorState::Destroy);
    }

    auto& colliders = GetGame()->GetColliders();
    for (auto* otherCollider : colliders) {
        // Só nos importamos com Inimigos
        if (otherCollider->GetLayer() == ColliderLayer::Enemy && otherCollider->IsEnabled()) {

            // Lógica de Intersecção AABB Manual
            Vector2 myMin = mCollider->GetMin();
            Vector2 myMax = mCollider->GetMax();
            Vector2 otherMin = otherCollider->GetMin();
            Vector2 otherMax = otherCollider->GetMax();

            bool noOverlap = myMax.x < otherMin.x ||
                             myMin.x > otherMax.x ||
                             myMax.y < otherMin.y ||
                             myMin.y > otherMax.y;

            if (!noOverlap) {
                // Houve colisão!
                Actor* enemy = otherCollider->GetOwner();
                if (enemy) {
                    enemy->TakeDamage(1);
                    SDL_Log("Bullet manual hit: %s", enemy->GetName());
                }
                SetState(ActorState::Destroy);
                return; // Sai da função pois a bala morreu
            }
        }
    }
}

void Bullet::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    if (other->GetLayer() == ColliderLayer::Blocks) {
        SetState(ActorState::Destroy);
    }
}