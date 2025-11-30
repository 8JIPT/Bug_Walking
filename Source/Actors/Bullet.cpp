#include "Bullet.h"
#include "../Game.h"
#include "../Components/Drawing/DrawComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Drawing/RectComponent.h"

Bullet::Bullet(Game* game, float direction)
    : Actor(game)
    , mSpeed(600.0f * direction)
    , mLiveTime(5.0f)
{
    // rect como placeholder
    mRect = new RectComponent(this, 10, 10, static_cast<RendererMode>(0), 110);
    mRect->SetColor(Vector3(255, 255, 0));

    mCollider = new AABBColliderComponent(this, 0, 0, 10, 10, ColliderLayer::Player, true, 10);
}

void Bullet::OnUpdate(float deltaTime) {
    Vector2 pos = GetPosition();
    pos.x += mSpeed * deltaTime;
    SetPosition(pos);

    mLiveTime -= deltaTime;
    if (mLiveTime <= 0.0f) {
        SetState(ActorState::Destroy);
    }
}

void Bullet::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    if (other->GetLayer() == ColliderLayer::Enemy) {
        Actor* enemy = other->GetOwner();
        if (enemy) {
            enemy->Kill();
        }
        SetState(ActorState::Destroy);
    }
    else if (other->GetLayer() == ColliderLayer::Blocks) {
        SetState(ActorState::Destroy);
    }
}