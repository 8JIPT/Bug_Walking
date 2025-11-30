//
// Created by pesso
//

#include "Missile.h"
#include "../Game.h"
#include "Mario.h"
#include "Robot.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/ParticleSystemComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Missile::Missile(Game* game, const Vector2& initialPosition, float moveSpeed, float lifetime,
                 const std::string& spritePath, const Vector3& particleColor)
    : Actor(game)
    , mMoveSpeed(moveSpeed)
    , mLifetime(lifetime)
    , mLifeTimer(lifetime)
    , mParticleEmitTimer(0.0f)
    , mParticleEmitRate(0.1f)
{
    SetPosition(initialPosition);

    mSpriteComponent = new AnimatorComponent(this, spritePath, "", Game::TILE_SIZE /2, Game::TILE_SIZE , 150);

    mFireTrail = new ParticleSystemComponent(this, Game::TILE_SIZE / 6, Game::TILE_SIZE / 6, 50);
    mFireTrail->SetColor(particleColor);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE / 3, Game::TILE_SIZE / 3, ColliderLayer::Enemy, false);
}

void Missile::OnUpdate(float deltaTime)
{
    mLifeTimer -= deltaTime;
    if (mLifeTimer <= 0.0f)
    {
        mState = ActorState::Destroy;
        return;
    }
    UpdateSteering(deltaTime);
    EmitFireTrail(deltaTime);
    float bottomLimit = Game::LEVEL_HEIGHT * Game::TILE_SIZE + Game::TILE_SIZE;
    if (mPosition.y > bottomLimit || mPosition.y < -Game::TILE_SIZE)
    {
        mState = ActorState::Destroy;
    }
}

void Missile::UpdateSteering(float deltaTime)
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return;
    
    Vector2 playerPos = player->GetPosition();
    Vector2 toPlayer = playerPos - mPosition;
    
    float distance = toPlayer.Length();
    if (distance > 1.0f)
    {
        toPlayer.Normalize();
        Vector2 desiredVelocity = toPlayer * mMoveSpeed;
        mRigidBodyComponent->SetVelocity(desiredVelocity);
        
        // Rotate to face direction
        float angle = Math::Atan2(desiredVelocity.y, desiredVelocity.x) + Math::PiOver2;
        SetRotation(angle);
    }
}

void Missile::EmitFireTrail(float deltaTime)
{
    mParticleEmitTimer += deltaTime;
    
    if (mParticleEmitTimer >= mParticleEmitRate)
    {
        Vector2 velocity = mRigidBodyComponent->GetVelocity();
        Vector2 behindOffset = Vector2::Zero;
        if (velocity.LengthSq() > 0.1f)
        {
            Vector2 direction = velocity;
            direction.Normalize();
            behindOffset = direction * -10.0f;
        }
        mFireTrail->EmitParticle(0.2f, 0.0f, behindOffset);
        mParticleEmitTimer = 0.0f;
    }
}

void Missile::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (!other) return;
    if (other->GetLayer() == ColliderLayer::Blocks)
    {
        SDL_Log("Missile hit block (horizontal)");
        mState = ActorState::Destroy;
        return;
    }
    if (other->GetLayer() == ColliderLayer::Player)
    {
        SDL_Log("Missile hit player (horizontal)!");
        mState = ActorState::Destroy;
        return;
    }
}

void Missile::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (!other) return;
    if (other->GetLayer() == ColliderLayer::Blocks)
    {
        SDL_Log("Missile hit block (vertical)");
        mState = ActorState::Destroy;
        return;
    }
    if (other->GetLayer() == ColliderLayer::Player)
    {
        SDL_Log("Missile hit player (vertical)!");
        mState = ActorState::Destroy;
        return;
    }
}

void Missile::Kill()
{
    SDL_Log("Missile destroyed");
    mState = ActorState::Destroy;
}
