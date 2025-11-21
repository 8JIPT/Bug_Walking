//
// Created by pesso on 24/10/2025.
//
// Mushroom.cpp
#include "Mushroom.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Mushroom::Mushroom(Game* game, const Vector2& startPos, float speed)
    : Actor(game), mSpeed(speed)
{
    SetPosition(startPos);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Collectables/Mushroom.png", "", Game::TILE_SIZE, Game::TILE_SIZE);
    mDrawComponent->SetIsPaused(true);

    mRigidBody = new RigidBodyComponent(this, 1.0f, 0.0f,true);
    mRigidBody->SetVelocity(Vector2(mSpeed, -250.0f));

    mCollider = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Item, false);
}

void Mushroom::OnUpdate(float deltaTime) {
    Vector2 pos = GetPosition();
    const float levelLeft   = 0.0f;
    const float levelTop    = 0.0f;
    const float levelRight  = static_cast<float>(Game::LEVEL_WIDTH) * static_cast<float>(Game::TILE_SIZE);
    const float levelBottom = static_cast<float>(Game::LEVEL_HEIGHT) * static_cast<float>(Game::TILE_SIZE);

    const float MARGIN = static_cast<float>(Game::TILE_SIZE) * Game::EPS;
    if (pos.x < levelLeft - MARGIN || pos.x > levelRight + MARGIN || pos.y < levelTop - MARGIN  || pos.y > levelBottom + MARGIN){
        SDL_Log("Mushroom leaving level -> destroy pos=(%.2f, %.2f) levelRight=%.2f levelBottom=%.2f", pos.x, pos.y, levelRight, levelBottom);
        mState = ActorState::Destroy;
    }
}

void Mushroom::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    if (!other) return;

    if (other->GetLayer() == ColliderLayer::Blocks) {
        Vector2 vel = mRigidBody->GetVelocity();
        vel.x = -vel.x;
        mRigidBody->SetVelocity(vel);
    }

    if (other->GetLayer() == ColliderLayer::Player) {
        mState = ActorState::Destroy;
        //mario handles his on powerUp
    }
}

void Mushroom::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {
    if (!other) return;

    if (other->GetLayer() == ColliderLayer::Blocks) {
        Vector2 vel = mRigidBody->GetVelocity();
        vel.y = 0.0f;
        mRigidBody->SetVelocity(vel);
        SetOnGround();
    }
    if (other->GetLayer() == ColliderLayer::Player) {
        mState = ActorState::Destroy;
        //mario handles his on powerUp
    }
}
