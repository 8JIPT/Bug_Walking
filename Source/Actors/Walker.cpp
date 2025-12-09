//
// Created by pesso
//

#include "Walker.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "Robot.h"

Walker::Walker(Game* game, const Vector2& spawnPosition, float moveSpeed, int maxHitPoints)
    : Actor(game)
    , mMoveSpeed(moveSpeed)
    , mMaxHitPoints(maxHitPoints)
    , mHitPoints(maxHitPoints)
    , mDyingTimer(0.5f)
    , mIsDying(false)
    , mDamageTimer(0.0f)
    , mTakingDamage(false)
    , mSpawnPosition(spawnPosition)
    , mPatrolDistance(50.0f)
    , mPauseTimer(0.0f)
    , mPauseTime(3.0f)
    , mMovingRight(true)
    , mMode(WalkerMode::Idle)
{
    SetPosition(spawnPosition);

    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Walker/Walker.png", "../Assets/Sprites/Walker/Walker.json", Game::TILE_SIZE * 2, Game::TILE_SIZE, 100);
    mDrawComponent->AddAnimation("idle", std::vector<int>{16, 18, 19, 32, 15});
    mDrawComponent->AddAnimation("walk", std::vector<int>{17, 6, 22, 23, 24, 20});
    mDrawComponent->AddAnimation("dead", std::vector<int>{3, 14, 5, 4, 7});
    mDrawComponent->AddAnimation("damage", std::vector<int>{12, 13, 11});
    mDrawComponent->SetAnimFPS(10.0f);
    mDrawComponent->SetAnimation("idle");

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f, true);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Enemy, false);
}

void Walker::OnUpdate(float deltaTime)
{
    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f)
        {
            mState = ActorState::Destroy;
        }
        return;
    }
    if (mTakingDamage)
    {
        mDamageTimer -= deltaTime;
        if (mDamageTimer <= 0.0f)
        {
            mTakingDamage = false;
            if (mMode == WalkerMode::Idle)
                mDrawComponent->SetAnimation("idle");
            else if (mMode == WalkerMode::Following)
                mDrawComponent->SetAnimation("walk");
            else if (mMode == WalkerMode::Battle)
                mDrawComponent->SetAnimation("idle");
        }
        return;
    }
    switch (mMode)
    {
        case WalkerMode::Idle:
            UpdateIdle(deltaTime);
            break;
        case WalkerMode::Following:
            UpdateFollowing(deltaTime);
            break;
        case WalkerMode::Battle:
            UpdateBattle(deltaTime);
            break;
    }

    // Check if fell off the world
    float bottomLimit = GetGame()->GetLevelHeight() * Game::TILE_SIZE + Game::TILE_SIZE;
    if (mPosition.y > bottomLimit)
    {
        Kill();
    }
}

void Walker::UpdateIdle(float deltaTime)
{
    if (CanSeePlayer())
    {
        mMode = WalkerMode::Following;
        mDrawComponent->SetAnimation("walk");
        return;
    }
    // Patrol behavior
    if (mPauseTimer > 0.0f)
    {
        mPauseTimer -= deltaTime;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mDrawComponent->SetAnimation("idle");
        if (mPauseTimer <= 0.0f)
        {
            mMovingRight = !mMovingRight;
            mDrawComponent->SetAnimation("walk");
        }
    }
    else
    {
        float distanceFromSpawn = mPosition.x - mSpawnPosition.x;
        if (mMovingRight && distanceFromSpawn >= mPatrolDistance)
        {
            mPauseTimer = mPauseTime;
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
        }
        else if (!mMovingRight && distanceFromSpawn <= -mPatrolDistance)
        {
            mPauseTimer = mPauseTime;
            mRigidBodyComponent->SetVelocity(Vector2::Zero);
        }
        else
        {
            Vector2 velocity = mRigidBodyComponent->GetVelocity();
            velocity.x = mMovingRight ? mMoveSpeed : -mMoveSpeed;
            mRigidBodyComponent->SetVelocity(velocity);
            mScale.x = mMovingRight ? 1.0f : -1.0f;
        }
    }
}

void Walker::UpdateFollowing(float deltaTime)
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
    {
        mMode = WalkerMode::Idle;
        return;
    }
    Vector2 playerPos = player->GetPosition();
    float horizontalDistance = Math::Abs(playerPos.x - mPosition.x);
    float verticalDistance = Math::Abs(playerPos.y - mPosition.y);
    float totalDistance = (playerPos - mPosition).Length();
    if (horizontalDistance <= ATTACK_DISTX && verticalDistance <= ATTACK_DISTY)
    {
        mMode = WalkerMode::Battle;
        mDrawComponent->SetAnimation("idle");
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        return;
    }
    if (!CanSeePlayer())
    {
        mMode = WalkerMode::Idle;
        mDrawComponent->SetAnimation("idle");
        return;
    }
    // Move toward player
    float horizontalDiff = playerPos.x - mPosition.x;
    Vector2 velocity = mRigidBodyComponent->GetVelocity();
    if (Math::Abs(horizontalDiff) > 15.0f)  // Dead zone
    {
        if (horizontalDiff > 0)
        {
            velocity.x = mMoveSpeed;
            mScale.x = 1.0f;
        }
        else
        {
            velocity.x = -mMoveSpeed;
            mScale.x = -1.0f;
        }
    }
    else
    {
        velocity.x = 0.0f;
    }
    mRigidBodyComponent->SetVelocity(velocity);
}

void Walker::UpdateBattle(float deltaTime)
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
    {
        mMode = WalkerMode::Idle;
        return;
    }
    Vector2 playerPos = player->GetPosition();
    float horizontalDistance = Math::Abs(playerPos.x - mPosition.x);
    float verticalDistance = Math::Abs(playerPos.y - mPosition.y);
    if (horizontalDistance > ATTACK_DISTX + 2* Game::TILE_SIZE || verticalDistance > ATTACK_DISTY + 2* Game::TILE_SIZE)
    {
        mMode = WalkerMode::Following;
        mDrawComponent->SetAnimation("walk");
        return;
    }
    float horizontalDiff = playerPos.x - mPosition.x;
    if (Math::Abs(horizontalDiff) > 10.0f)
    {
        mScale.x = (horizontalDiff > 0.0f) ? 1.0f : -1.0f;
    }
    Vector2 velocity = mRigidBodyComponent->GetVelocity();
    if (Math::Abs(horizontalDiff) > 10.0f)
    {
        float direction = (horizontalDiff > 0.0f) ? 1.0f : -1.0f;
        mScale.x = direction;
        velocity.x = direction * 30.0f;
        mDrawComponent->SetAnimation("walk");
    }
    else
    {
        velocity.x = 0.0f;
        mDrawComponent->SetAnimation("idle");
    }
    mRigidBodyComponent->SetVelocity(velocity);
}

bool Walker::CanSeePlayer()
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return false;
    Vector2 playerPos = player->GetPosition();
    float distance = (playerPos - mPosition).Length();
    return distance <= DETECTION_RANGE;
}

void Walker::TakeDamage(int damage)
{
    if (mIsDying || mTakingDamage)
        return;
    mHitPoints -= damage;
    if (mHitPoints <= 0)
    {
        mHitPoints = 0;
        Kill();
    }
    else
    {
        mTakingDamage = true;
        mDamageTimer = 0.3f;
        mDrawComponent->SetAnimation("damage");
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        SDL_Log("Walker playing damage animation");
    }
}

void Walker::Kill()
{
    if (mIsDying)
        return;
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Walker::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying)
        return;
    if (other->GetLayer() == ColliderLayer::Blocks)
    {
        if (mMode == WalkerMode::Idle)
        {
            mMovingRight = !mMovingRight;
            mPauseTimer = 0.0f;
        }
    }
    else if (other->GetLayer() == ColliderLayer::Enemy)
    {
        Actor* otherActor = other->GetOwner();
        if (otherActor && strcmp(otherActor->GetName(), "Walker") == 0)
        {
            if (mMode == WalkerMode::Idle)
            {
                mMovingRight = !mMovingRight;
                mPauseTimer = 0.0f;
            }
        }
    }
}

void Walker::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying)
        return;
    if (other->GetLayer() == ColliderLayer::Player)
    {
        if (minOverlap > 0.0f)
        {
            TakeDamage(1);
        }
    }
}
