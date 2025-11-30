//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Mario.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

Mario::Mario(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
{
    mNormalDraw = new AnimatorComponent(this, "../Assets/Sprites/Mario/Mario.png", "../Assets/Sprites/Mario/Mario.json", Game::TILE_SIZE, Game::TILE_SIZE, 100);

    mNormalDraw->AddAnimation("dead", std::vector<int>{0});
    mNormalDraw->AddAnimation("idle", std::vector<int>{1});
    mNormalDraw->AddAnimation("jump", std::vector<int>{2});
    mNormalDraw->AddAnimation("run",  std::vector<int>{3, 4, 5});

    mNormalDraw->SetAnimFPS(10.0f);
    mNormalDraw->SetAnimation("idle");

    mSuperDraw = new AnimatorComponent(this, "../Assets/Sprites/SuperMario/SuperMario.png","../Assets/Sprites/SuperMario/SuperMario.json", Game::TILE_SIZE, Game::TILE_SIZE * 2, 100);
    mSuperDraw->AddAnimation("idle", std::vector<int>{0});
    mSuperDraw->AddAnimation("jump", std::vector<int>{1});
    mSuperDraw->AddAnimation("run",  std::vector<int>{2, 3, 4});

    mSuperDraw->SetAnimFPS(10.0f);
    mSuperDraw->SetAnimation("idle");
    mSuperDraw->SetVisible(false);


    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 5.0f);


    mColliderComponent = new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Player, false);
}

void Mario::OnProcessInput(const uint8_t* state)
{
    mIsRunning = false;

    if (state[SDL_SCANCODE_D]){
        mRigidBodyComponent->ApplyForce(Vector2(mForwardSpeed, 0.0f));
        mScale.x = 1.0f;
        mIsRunning = true;
    }
    else if (state[SDL_SCANCODE_A]){
        mRigidBodyComponent->ApplyForce(Vector2(-mForwardSpeed, 0.0f));
        mScale.x = -1.0f;
        mIsRunning = true;
    }

    if (state[SDL_SCANCODE_SPACE] && IsOnGround()){
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.y = mJumpSpeed;
        mRigidBodyComponent->SetVelocity(vel);
        SetOffGround();
    }
}

void Mario::OnUpdate(float deltaTime)
{
    if (mHitTimer > 0.0f){
        mHitTimer -= deltaTime;

        bool visible = (int)(mHitTimer * 10) % 2 == 0;
        mNormalDraw->SetVisible(visible && !mIsSuper);
        mSuperDraw->SetVisible(visible && mIsSuper);
    } else{
        mNormalDraw->SetVisible(!mIsSuper);
        mSuperDraw->SetVisible(mIsSuper);
    }
    mHitThisFrame = false;

    if (mRigidBodyComponent->GetVelocity().y != 0.0f) SetOffGround();

    if (mPosition.y > Game::LEVEL_HEIGHT * Game::TILE_SIZE) {
        PowerDown();
        Kill();
    }

    ManageAnimations();

    // Clamp Mario to level boundaries
    float levelWidth = Game::LEVEL_WIDTH * Game::TILE_SIZE;
    float halfTile = Game::TILE_SIZE * 0.5f;
    
    // Left boundary
    if (mPosition.x < halfTile) {
        mPosition.x = halfTile;
        Vector2 v = mRigidBodyComponent->GetVelocity();
        v.x = 0.0f;
        mRigidBodyComponent->SetVelocity(v);
    }
    
    // Right boundary
    if (mPosition.x > levelWidth - halfTile) {
        mPosition.x = levelWidth - halfTile;
        Vector2 v = mRigidBodyComponent->GetVelocity();
        v.x = 0.0f;
        mRigidBodyComponent->SetVelocity(v);
    }

}

void Mario::ManageAnimations()
{
    if (mIsDead){
        mNormalDraw->SetAnimation("dead");
        return;
    }

    if (!IsOnGround()){
        mNormalDraw->SetAnimation("jump");
        mSuperDraw->SetAnimation("jump");
        return;
    }

    if (mIsRunning){
        mNormalDraw->SetAnimation("run");
        mSuperDraw->SetAnimation("run");
    } else{
        mNormalDraw->SetAnimation("idle");
        mSuperDraw->SetAnimation("idle");
    }
}

void Mario::Kill()
{
    if (mIsDead) return;

    mIsDead = true;
    mNormalDraw->SetAnimation("dead");

    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    SDL_Log("Killed");
}

void Mario::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (!other || mHitThisFrame) return;
    if (other->GetLayer() == ColliderLayer::Enemy && mHitTimer <= 0.0f){
        mHitThisFrame = true;
        if (Actor* enemy = other->GetOwner()) {
            const char* enemyName = enemy->GetName();
            if (strcmp(enemyName, "Missile") == 0) {
                enemy->Kill();
                SDL_Log("Mario killed Missile on horizontal collision");
            }
        }
        
        if (mIsSuper){
            PowerDown();
        } else{
            Kill();
        }
        mHitTimer = HIT_COOLDOWN;
    }
    if (other->GetLayer() == ColliderLayer::Item){
        PowerUp();
    }
}

void Mario::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (!other) return;
    if (other->GetLayer() == ColliderLayer::Blocks){
        Vector2 marioMin = mColliderComponent->GetMin();
        Vector2 marioMax = mColliderComponent->GetMax();
        Vector2 blockMin = other->GetMin();
        Vector2 blockMax = other->GetMax();

        // bump block if hit from below
        if (marioMin.y >= blockMax.y - Game::EPS){
            if (Actor* block = other->GetOwner()){
                block->Kill();
            }
        }
    }

    if (other->GetLayer() == ColliderLayer::Item){
        PowerUp();
    }
    if (other->GetLayer() != ColliderLayer::Enemy) return;


    Vector2 marioMin = mColliderComponent->GetMin();
    Vector2 marioMax = mColliderComponent->GetMax();
    float marioFeetY = marioMax.y; // bottom of Mario

    Vector2 enemyMin = other->GetMin();
    Vector2 enemyMax = other->GetMax();
    float enemyCenterY = (enemyMin.y + enemyMax.y) * 0.5f;


    //stomp only if mario is above
    if (marioFeetY < enemyCenterY){
        if (Actor* enemy = other->GetOwner()) enemy->TakeDamage(1);
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.y = mJumpSpeed * -0.5f;
        mRigidBodyComponent->SetVelocity(vel);
        return;
    }

    if (mHitTimer <= 0.0f) {
        mHitThisFrame = true;
        if (Actor* enemy = other->GetOwner()) {
            const char* enemyName = enemy->GetName();
            if (strcmp(enemyName, "Missile") == 0) {
                enemy->Kill();
                SDL_Log("Mario killed Missile on vertical collision");
            }
        }
        
        if (mIsSuper){
            PowerDown();
        }else{
            Kill();
        }
        mHitTimer = HIT_COOLDOWN;
    }
}

void Mario::PowerUp(){
    if (!mIsSuper) {
        mIsSuper = true;
        mNormalDraw->SetVisible(false);
        mSuperDraw->SetVisible(true);
        mColliderComponent->SetSize(Game::TILE_SIZE, Game::TILE_SIZE * 2);
        SDL_Log("Mario is Super");
    }
}

void Mario::PowerDown(){
    if (mIsSuper) {
        mIsSuper = false;
        mNormalDraw->SetVisible(true);
        mSuperDraw->SetVisible(false);
        mColliderComponent->SetSize(Game::TILE_SIZE, Game::TILE_SIZE);
        SDL_Log("Mario is Normal");
    }
}