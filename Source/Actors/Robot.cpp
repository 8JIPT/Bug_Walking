// Robot.cpp
#include "Robot.h"
#include "../Game.h"
#include "../UI/Screens/HUD.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"
#include  "Bullet.h"
#include "GoldRing.h"
#include  "../Random.h"
#include "SmokeEffect.h"

Robot::Robot(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
        , mShootCooldown(0.3f)
        , mShootCooldownTimer(0.0f)
        , mGlitchDurationTimer(0.0f)
        , mIsInputLocked(false)
        , mCurrentLevel(RepairLevel::Critical) // Garante que começa em Critical
{
    mNormalDraw = new AnimatorComponent(this, "../Assets/Sprites/Robot/Character_SpriteSheet_RP1_free (40x40).png", "../Assets/Sprites/Robot/Robot.json", Game::TILE_SIZE * 2, Game::TILE_SIZE * 2, 100);
    mNormalDraw->SetOffset(Vector2(0, Game::TILE_SIZE * -0.4));
    mNormalDraw->AddAnimation("idle", std::vector<int>{0,1,2,3,4});
    mNormalDraw->AddAnimation("run", std::vector<int>{5,6,7,8,9,10,11,12});
    mNormalDraw->AddAnimation("shot", std::vector<int>{13,14});
    mNormalDraw->AddAnimation("dead", std::vector<int>{15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35});
    mNormalDraw->AddAnimation("jump", std::vector<int>{36,37,38,39});
    mNormalDraw->SetAnimFPS(10.0f);
    mNormalDraw->SetAnimation("idle");
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE * 0.8, Game::TILE_SIZE * 1.3, ColliderLayer::Player, false);
    SetRepairLevel(RepairLevel::Critical);
}

void Robot::SetRepairLevel(RepairLevel level) {
    mCurrentLevel = level;
    switch (level) {
    case RepairLevel::Critical:
        mTimeBetweenGlitches = 10.0f;
        mShootFailChance = 0.5f;
        SDL_Log("[RepairLevel] Mudou para CRITICAL");
        break;
    case RepairLevel::Damaged:
        mTimeBetweenGlitches = 20.0f;
        mShootFailChance = 0.25f;
        SDL_Log("[RepairLevel] Mudou para DAMAGED");
        break;
    case RepairLevel::Fixed:
        mTimeBetweenGlitches = 99999.0f;
        mShootFailChance = 0.0f;
        SDL_Log("[RepairLevel] Mudou para FIXED");
        break;
    }
    mGlitchTimer = mTimeBetweenGlitches;
}

void Robot::OnProcessInput(const uint8_t* state)
{
    mIsRunning = false;
    if (mIsDead || mIsInputLocked) {
        return;
    }
    if (state[SDL_SCANCODE_RIGHT]){
        mRigidBodyComponent->ApplyForce(Vector2(mForwardSpeed, 0.0f));
        mScale.x = 1.0f;
        mIsRunning = true;
    }
    else if (state[SDL_SCANCODE_LEFT]){
        mRigidBodyComponent->ApplyForce(Vector2(-mForwardSpeed, 0.0f));
        mScale.x = -1.0f;
        mIsRunning = true;
    }
    else {
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.x = 0.0f;
        mRigidBodyComponent->SetVelocity(vel);
    }
    if (state[SDL_SCANCODE_UP] && IsOnGround()){
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.y = mJumpSpeed;
        mRigidBodyComponent->SetVelocity(vel);
        SetOffGround();
        GetGame()->PlayJumpChunk();
    }
    if (state[SDL_SCANCODE_SPACE]) {
        HandleShooting();
    }
}

void Robot::OnUpdate(float deltaTime)
{
    if (mHitTimer > 0.0f){
        mHitTimer -= deltaTime;
        bool visible = (int)(mHitTimer * 10) % 2 == 0;
        mNormalDraw->SetVisible(visible);
    } else{
        mNormalDraw->SetVisible(true);
    }
    mHitThisFrame = false;
    if (mStompedEnemy) {
        bool stillColliding = false;
        if (mColliderComponent && mColliderComponent->IsEnabled()) {
            auto colliders = GetGame()->GetColliders();
            for (auto collider : colliders) {
                if (collider->GetOwner() == mStompedEnemy && 
                    mColliderComponent->Intersect(*collider)) {
                    stillColliding = true;
                    break;
                }
            }
        }
        if (!stillColliding) {
            mStompedEnemy = nullptr;
        }
    }
    
    if (mRigidBodyComponent->GetVelocity().y != 0.0f) SetOffGround();

    float winY = GetGame()->GetWinConditionY();
    float winX = GetGame()->GetWinConditionX();
    bool wonByY = (winY >= 0 && mPosition.y <= winY);
    bool wonByX = (winX >= 0 && mPosition.x >= winX);
    if ((wonByY || wonByX) && !mIsDead) {
        GetGame()->SetScene(GameScene::Win);
        return;
    }
    if (mPosition.y > GetGame()->GetLevelHeight() * Game::TILE_SIZE) {
        Kill();
    }
    if (mShootCooldownTimer > 0.0f) {
        mShootCooldownTimer -= deltaTime;
    }
    if (!mIsDead) {
        HandleGlitches(deltaTime);
    }
    ManageAnimations();
    float levelWidth = GetGame()->GetLevelWidth() * Game::TILE_SIZE;
    float halfTile = 20.0f; // metade do tamanho do Robot
    if (mPosition.x < halfTile) {
        mPosition.x = halfTile;
        Vector2 v = mRigidBodyComponent->GetVelocity();
        v.x = 0.0f;
        mRigidBodyComponent->SetVelocity(v);
    }
    if (mPosition.x > levelWidth - halfTile) {
        mPosition.x = levelWidth - halfTile;
        Vector2 v = mRigidBodyComponent->GetVelocity();
        v.x = 0.0f;
        mRigidBodyComponent->SetVelocity(v);
    }
}

void Robot::HandleGlitches(float deltaTime) {
    if (mCurrentLevel == RepairLevel::Fixed) return;
    if (mIsInputLocked) {
        mGlitchDurationTimer -= deltaTime;
        if (mGlitchDurationTimer <= 0.0f) {
            mIsInputLocked = false;
            SDL_Log("Sistema recuperado do Glitch.");
        }
        return;
    }
    mGlitchTimer -= deltaTime;
    if (mGlitchTimer <= 0.0f) {
        mIsInputLocked = true;
        mGlitchDurationTimer = 1.0f;
        mGlitchTimer = mTimeBetweenGlitches;
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.x = 0.0f;
        mRigidBodyComponent->SetVelocity(vel);
        GetGame()->PlayGlitchChunk();
        SDL_Log("GLITCH: Input de movimento perdido!");
    }
}

void Robot::HandleShooting() {
    if (mShootCooldownTimer > 0.0f) return;
    mShootCooldownTimer = mShootCooldown;
    float direction = mScale.x;
    Vector2 spawnPos = GetPosition();
    spawnPos.x += (25.0f) * direction;
    spawnPos.y += -3.0f;
    float roll = Random::GetFloat();
    if (roll < mShootFailChance) {
        SDL_Log("GLITCH: Arma falhou ao disparar!");
        new SmokeEffect(GetGame(), spawnPos, direction);
        GetGame()->PlayFailedShotChunk();
        return;
    }
    Bullet* bullet = new Bullet(GetGame(), direction);
    bullet->SetPosition(spawnPos);
    GetGame()->PlayShootChunk();
}

void Robot::ManageAnimations()
{
    if (mIsDead){
        mNormalDraw->SetAnimation("dead");
        return;
    }
    if (!IsOnGround()){
        mNormalDraw->SetAnimation("jump");
        return;
    }
    if (mIsRunning){
        mNormalDraw->SetAnimation("run");
    } else{
        mNormalDraw->SetAnimation("idle");
    }
}

void Robot::TakeDamage(int damage)
{
    if (mIsDead || mHitTimer > 0.0f)
        return;
    mHitPoints -= damage;
    mGame->GetAudio()->PlaySound("hurt.wav");
    // Update HUD
    if (GetGame()->GetHUD())
    {
        GetGame()->GetHUD()->SetHealth(mHitPoints);
    }
    if (mHitPoints <= 0)
    {
        mHitPoints = 0;
        Kill();
    }
    else
    {
        mHitTimer = HIT_COOLDOWN;
        mHitThisFrame = true;
    }
}

void Robot::Kill()
{
    if (mIsDead) return;
    mIsDead = true;
    mNormalDraw->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Robot::UpgradeRepairLevel() {
    if (mCurrentLevel == RepairLevel::Critical) {
        SetRepairLevel(RepairLevel::Damaged);
        SDL_Log("RepairLevel up: Damaged");
    } else if (mCurrentLevel == RepairLevel::Damaged) {
        SetRepairLevel(RepairLevel::Fixed);
        SDL_Log("RepairLevel up: Fixed");
    } else {
        SDL_Log("RepairLevel já está no máximo!");
    }
}

void Robot::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (!other || mHitThisFrame) return;
    if (other->GetLayer() == ColliderLayer::Enemy && mHitTimer <= 0.0f){
        Actor* enemy = other->GetOwner();
        if (enemy == mStompedEnemy) {
            return;
        }
        mHitThisFrame = true;
        if (enemy) {
            const char* enemyName = enemy->GetName();
            if (strcmp(enemyName, "Missile") == 0) {
                enemy->Kill();
            }
        }
        TakeDamage(1);
    }
    if (other->GetLayer() == ColliderLayer::Item) {
        Actor* item = other->GetOwner();
        if (item && strcmp(item->GetName(), "GoldRing") == 0) {
            GoldRing* ring = dynamic_cast<GoldRing*>(item);
            if (ring) {
                UpgradeRepairLevel();
                ring->SetState(ActorState::Destroy);
            }
        }
        // PowerUp(); // Se quiser implementar powerup para Robot
    }
}

void Robot::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (!other) return;
    if (other->GetLayer() == ColliderLayer::Blocks){
        Vector2 robotMin = mColliderComponent->GetMin();
        Vector2 robotMax = mColliderComponent->GetMax();
        Vector2 blockMin = other->GetMin();
        Vector2 blockMax = other->GetMax();
        if (robotMin.y >= blockMax.y - Game::EPS){
            if (Actor* block = other->GetOwner()){
                block->Kill();
            }
        }
    }
    if (other->GetLayer() == ColliderLayer::Item){
        // PowerUp(); // Se quiser implementar powerup para Robot
    }
    if (other->GetLayer() != ColliderLayer::Enemy) return;
    
    Actor* enemy = other->GetOwner();
    if (!enemy) return;
    
    Vector2 robotMin = mColliderComponent->GetMin();
    Vector2 robotMax = mColliderComponent->GetMax();
    float robotFeetY = robotMax.y;
    Vector2 enemyMin = other->GetMin();
    Vector2 enemyMax = other->GetMax();
    float enemyCenterY = (enemyMin.y + enemyMax.y) * 0.5f;
    Vector2 velocity = mRigidBodyComponent->GetVelocity();
    bool isFalling = velocity.y > 0.0f;
    if (robotFeetY < enemyCenterY && isFalling && minOverlap > 0){
        // Stomp detected - let the enemy handle its own damage
        enemy->OnVerticalCollision(minOverlap, mColliderComponent);
        mStompedEnemy = enemy; // Grant immunity from this enemy for the time its still inside it
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.y = mJumpSpeed * -0.5f;
        mRigidBodyComponent->SetVelocity(vel);
        return;
    }
    if (minOverlap < 0) {
        return;
    }
    Vector2 enemyPos = enemy->GetPosition();
    if (mPosition.y < enemyPos.y - 10.0f || isFalling) {
        return;
    }
    if (mStompedEnemy == enemy) {
        return;
    }
    if (mHitTimer <= 0.0f) {
        mHitThisFrame = true;
        const char* enemyName = enemy->GetName();
        if (strcmp(enemyName, "Missile") == 0) {
            enemy->Kill();
        }
        TakeDamage(1);
    }
}
