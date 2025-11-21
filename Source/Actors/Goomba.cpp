//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Goomba.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Goomba::Goomba(Game* game, float forwardSpeed, float deathTime)
        : Actor(game)
        , mDyingTimer(deathTime)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
{
        mDrawComponent = new AnimatorComponent(this,"../Assets/Sprites/Goomba/Goomba.png","../Assets/Sprites/Goomba/Goomba.json",Game::TILE_SIZE, Game::TILE_SIZE, 100);
        mDrawComponent->AddAnimation("dead", std::vector<int>{0});
        mDrawComponent->AddAnimation("walk", std::vector<int>{1, 2});

        mDrawComponent->SetAnimFPS(10.0f);
        mDrawComponent->SetAnimation("walk");

        mRigidBodyComponent = new RigidBodyComponent(this);

        mColliderComponent = new AABBColliderComponent(this,0, 0,Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Enemy,false);

        mRigidBodyComponent->SetVelocity(Vector2(-mForwardSpeed, 0.0f));
}

void Goomba::Kill()
{
        if (mIsDying) return;

        mIsDying = true;
        mDrawComponent->SetAnimation("dead");

        mRigidBodyComponent->SetEnabled(false);
        mColliderComponent->SetEnabled(false);

}

void Goomba::OnUpdate(float deltaTime)
{
        if (mIsDying){
                mDyingTimer -= deltaTime;
                if (mDyingTimer <= 0.0f){
                        mState = ActorState::Destroy;
                }
                return;
        }

        float bottomLimit = Game::LEVEL_HEIGHT * Game::TILE_SIZE + Game::TILE_SIZE;
        if (mPosition.y > bottomLimit && !mIsDying){
                SDL_Log("Goomba y=%.2f bottomLimit=%.2f", mPosition.y, bottomLimit);
                Kill();
        }
}

void Goomba::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
        if (mIsDying) return;

        if (other->GetLayer() == ColliderLayer::Player){
                // mario handles his own death
                return;
        }
        mScale.x *= -1.0f;
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.x = (minOverlap  > 0.0f) ? -mForwardSpeed : mForwardSpeed;
        mRigidBodyComponent->SetVelocity(vel);
}

void Goomba::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{

}
