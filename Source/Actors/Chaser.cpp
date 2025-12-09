//
// Created by pesso
//

#include "Chaser.h"
#include "../Game.h"
#include "Robot.h"
#include "Missile.h"
#include "../Random.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Chaser::Chaser(Game* game, float moveSpeed, int maxHitPoints)
    : Actor(game)
    , mMoveSpeed(moveSpeed)
    , mMaxHitPoints(maxHitPoints)
    , mHitPoints(maxHitPoints)
    , mDyingTimer(1.0f)
    , mIsDying(false)
    , mDamageTimer(0.0f)
    , mPreviousAnimation("move")
    , mOffsetDistance(100.0f)
    , mTargetPosition(Vector2::Zero)
    , mMode(ChaserMode::Following)
    , mCooldownTimer(0.0f)
    , mMissileFireTimer(0.0f)
    , mMissileFireRate(7.0f)
    , mBattleMissileFireRate(3.5f)
    , mLastPlayerPosition(Vector2::Zero)
{
    // death frames: 0,2,4,25,26,27,30,31  | 3fire frames: 1,3,5,6,7,8,9,10,12,13,14,15,17,18,19,20 | forward frames: 11,15,16,21
    mDrawComponent = new AnimatorComponent(this, "../Assets/Sprites/Chaser/Chaser.png", "../Assets/Sprites/Chaser/Chaser.json", Game::TILE_SIZE, Game::TILE_SIZE, 150);
    mDrawComponent->AddAnimation("move",std::vector<int>{11, 15, 16, 21});
    mDrawComponent->AddAnimation("battle",std::vector<int>{12, 13, 14, 17, 18, 19, 20, 22});
    mDrawComponent->AddAnimation("damage",std::vector<int>{26, 2});
    mDrawComponent->AddAnimation("dead",std::vector<int>{35,4, 5, 0, 27, 30, 31, 32});

    mDrawComponent->SetAnimFPS(8.0f);
    mDrawComponent->SetAnimation("move");

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, false);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Enemy, false);

    float randomY = (Random::GetFloatRange(0.0f, 1.0f) - 0.5f) * 100.0f;
    mOffsetDistance += Random::GetFloatRange(-30.0f, 30.0f);
    mRandomYOffset = randomY;
    mOffsetSide = (Random::GetFloatRange(0.0f, 1.0f) > 0.5f) ? 1 : -1;
}

void Chaser::OnUpdate(float deltaTime)
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

    if (mDamageTimer > 0.0f)
    {
        mDamageTimer -= deltaTime;
        if (mDamageTimer <= 0.0f)
        {
            // Restore previous animation
            mDrawComponent->SetAnimation(mPreviousAnimation);
        }
    }

    switch (mMode)
    {
        case ChaserMode::Following:
            UpdateSteering(deltaTime);
            break;
        case ChaserMode::Battle:
            UpdateBattle(deltaTime);
            break;
        case ChaserMode::Cooldown:
            UpdateCooldown(deltaTime);
            break;
    }
}

void Chaser::UpdateSteering(float deltaTime)
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return;

    mTargetPosition = CalculateTargetPosition();

    Vector2 toTarget = mTargetPosition - mPosition;
    float distance = toTarget.Length();

    //Fire while following
    mMissileFireTimer += deltaTime;
    if (mMissileFireTimer >= mMissileFireRate)
    {
        // Normal missile
        mGame->GetAudio()->PlaySound("Shoot5.wav");
        new Missile(GetGame(), mPosition, 100.0f, 5.0f, "../Assets/Sprites/Chaser/roc.png", Vector3(1.0f, 0.5f, 0.0f));
        SDL_Log("Chaser fired normal missile!");
        mMissileFireTimer = 0.0f;
    }

    // If we're not at the target, move towards it
    if (distance > 10.0f)
    {
        toTarget.Normalize();
        Vector2 desiredVelocity = toTarget * mMoveSpeed;
        mRigidBodyComponent->SetVelocity(desiredVelocity);

        // Flip sprite based on direction
        if (desiredVelocity.x < 0)
        {
            mScale.x = -1.0f;
        }
        else if (desiredVelocity.x > 0)
        {
            mScale.x = 1.0f;
        }
    }
    else
    {
        // Reached target
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mMode = ChaserMode::Battle;
        mDrawComponent->SetAnimation("battle");
        mLastPlayerPosition = player->GetPosition();
        SDL_Log("Chaser entered battle mode!");
    }
}

void Chaser::UpdateBattle(float deltaTime)
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return;

    if (HasPlayerMoved())
    {
        mMode = ChaserMode::Cooldown;
        mCooldownTimer = COOLDOWN_TIME;
        mDrawComponent->SetAnimation("move");
        SDL_Log("Player moved! Chaser entering cooldown...");
        return;
    }

    // Fire missiles at faster rate with increased speed in battle mode
    mMissileFireTimer += deltaTime;
    if (mMissileFireTimer >= mBattleMissileFireRate)
    {
        // Battle missile
        mGame->GetAudio()->PlaySound("Shoot5.wav");
        new Missile(GetGame(), mPosition, 150.0f, 2.5f, "../Assets/Sprites/Chaser/storm_shadow.png", Vector3(1.0f, 0.0f, 0.5f));
        SDL_Log("Chaser fired fast battle missile!");
        mMissileFireTimer = 0.0f;
    }
}

void Chaser::UpdateCooldown(float deltaTime)
{
    mCooldownTimer -= deltaTime;
    
    if (mCooldownTimer <= 0.0f)
    {
        // Cooldown finished
        mMode = ChaserMode::Following;
        SDL_Log("Chaser cooldown finished, resuming following!");
    }
}

bool Chaser::HasPlayerMoved()
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return false;

    Vector2 currentPlayerPos = player->GetPosition();
    float distance = (currentPlayerPos - mLastPlayerPosition).Length();

    return distance > 20.0f;
}



Vector2 Chaser::CalculateTargetPosition()
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return mPosition;

    Vector2 playerPos = player->GetPosition();
    Vector2 offset(mOffsetSide * mOffsetDistance, mRandomYOffset);

    return playerPos + offset;
}

void Chaser::TakeDamage(int damage)
{
    if (mIsDying)
        return;

    mHitPoints -= damage;
    SDL_Log("Chaser hit! HP: %d/%d", mHitPoints, mMaxHitPoints);

    if (mHitPoints <= 0)
    {
        Kill();
    }
    else
    {
        if (mMode == ChaserMode::Following)
            mPreviousAnimation = "move";
        else if (mMode == ChaserMode::Battle)
            mPreviousAnimation = "battle";
        
        mDamageTimer = 0.2f;
        mDrawComponent->SetAnimation("damage");
    }
}

void Chaser::Kill()
{
    if (mIsDying)
        return;

    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    SDL_Log("Chaser killed!");
}

void Chaser::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying)
        return;

    //Don't resolve block collisions - pass through them
    if (other->GetLayer() == ColliderLayer::Blocks)
    {
        // Undo the collision resolution that was already applied
        Vector2 pos = mPosition;
        pos.x += minOverlap;
        SetPosition(pos);
        return;
    }
    if (other->GetLayer() == ColliderLayer::Player)
    {
        if (minOverlap < 0.0f)
        {
            TakeDamage(1);
        }
    }
}

void Chaser::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying)
        return;

    // Don't resolve block collisions - pass through them
    if (other->GetLayer() == ColliderLayer::Blocks)
    {
        // Undo the collision resolution that was already applied
        Vector2 pos = mPosition;
        pos.y += minOverlap;
        SetPosition(pos);
        return;
    }

    if (other->GetLayer() == ColliderLayer::Player)
    {
        // If player lands on top, take damage
        if (minOverlap < 0.0f)
        {
            TakeDamage(1);
        }
    }
}
