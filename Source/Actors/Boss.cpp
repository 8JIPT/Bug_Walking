//
// Boss.cpp - Implementação simplificada do chefão
//

#include "Boss.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "Robot.h"
#include "BossBullet.h"
#include "Missile.h"
#include "../AudioSystem.h"

Boss::Boss(Game* game, const Vector2& spawnPosition, float moveSpeed, int maxHitPoints)
    : Actor(game)
    , mMoveSpeed(moveSpeed)
    , mMaxHitPoints(maxHitPoints)
    , mHitPoints(maxHitPoints)
    , mDyingTimer(0.8f)
    , mIsDying(false)
    , mSpawnPosition(spawnPosition)
    , mPatrolDistance(150.0f)
    , mPatrolDirection(1)
    , mState(BossState::Patrol)
    , mShootTimer(0.0f)
    , mShootCooldown(2.0f)
    , mMissileTimer(0.0f)
    , mMissileCooldown(5.0f)
{
    SetPosition(spawnPosition);

    // Carregar animações do boss
    mAnimatorComponent = new AnimatorComponent(this, "../Assets/Sprites/Boss/Enemy03_SpriteSheet_RP1 (32x36).png",
                                               "../Assets/Sprites/Boss/Boss.json",
                                               32, 36, 100);

    mAnimatorComponent->AddAnimation("walk", std::vector<int>{0, 1, 2, 3});
    mAnimatorComponent->AddAnimation("dead", std::vector<int>{21, 22, 23});

    mAnimatorComponent->SetAnimFPS(5.0f);
    // Fixa animação para evitar flicker
    mAnimatorComponent->SetAnimation("walk");
    // Offset para levantar o sprite do chão (compensado para escala 3x)
    mAnimatorComponent->SetOffset(Vector2(0.0f, -37.0f));

    // Definir escala visual do boss para 3x
    mScale = Vector2(3.0f, 3.0f);

    // Componente de física (com gravidade para ficar no chão)
    mRigidBodyComponent = new RigidBodyComponent(this, 1.2f, 8.0f, true);

    // Colisor principal do boss (mantido no tamanho original 32x36)
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 32, 36,
                                                   ColliderLayer::Enemy, false);

    SDL_Log("BOSS spawned at (%.2f, %.2f)", spawnPosition.x, spawnPosition.y);
}

void Boss::OnUpdate(float deltaTime)
{
    if (mIsDying)
    {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f)
        {
            SetState(ActorState::Destroy);
        }
        return;
    }

    // Removido: alinhamento da base do boss com a base do player
    // if (GetGame()->GetPlayer()) {
    //     Vector2 p = GetGame()->GetPlayer()->GetPosition();
    //     mPosition.y = p.y;
    // }

    // Atualizar timer de tiro
    mShootTimer += deltaTime;
    
    // Atualizar timer de mísseis
    mMissileTimer += deltaTime;

    // Patrulhar
    Vector2 velocity = mRigidBodyComponent->GetVelocity();
    
    // Movimento de patrulha
    float distanceFromSpawn = Math::Abs(mPosition.x - mSpawnPosition.x);
    if (distanceFromSpawn > mPatrolDistance)
    {
        // Inverter direção
        mPatrolDirection *= -1;
    }
    
    velocity.x = mMoveSpeed * mPatrolDirection;
    mRigidBodyComponent->SetVelocity(velocity);
    
    // Atualizar escala baseado na direção (mantendo escala 3x)
    mScale.x = mPatrolDirection > 0 ? 3.0f : -3.0f;
    mScale.y = 3.0f;

    // Verificar se pode atirar no jogador
    if (CanSeePlayer() && mShootTimer >= mShootCooldown)
    {
        Shoot();
        mShootTimer = 0.0f;
    }
    
    // Lançar míssil a cada 5 segundos
    if (mMissileTimer >= mMissileCooldown)
    {
        LaunchMissile();
        mMissileTimer = 0.0f;
    }

    // Verificar se caiu do mundo
    float bottomLimit = GetGame()->GetLevelHeight() * Game::TILE_SIZE + Game::TILE_SIZE * 2;
    if (mPosition.y > bottomLimit)
    {
        TakeDamage(mHitPoints);
    }
}

void Boss::Shoot()
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return;

    // Criar projétil do Boss
    float shootDirection = mScale.x > 0.0f ? 1.0f : -1.0f;
    BossBullet* bullet = new BossBullet(GetGame(), shootDirection);
    
    float tile = static_cast<float>(Game::TILE_SIZE);
    Vector2 bulletOffset = Vector2(shootDirection > 0.0f ? tile : -tile, 0.0f);
    bullet->SetPosition(mPosition + bulletOffset);

    // Tocar som de tiro
    if (GetGame()->GetAudio())
    {
        GetGame()->GetAudio()->PlaySound("Shoot.wav");
    }

    SDL_Log("BOSS shooting at player");
}

void Boss::LaunchMissile()
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return;

    // Lançar míssil acima do Boss
    Vector2 missilePos = mPosition + Vector2(0.0f, -Game::TILE_SIZE * 2.0f);
    new Missile(GetGame(), missilePos, 200.0f, 8.0f, 
                "../Assets/Sprites/Chaser/roc.png", 
                Vector3(1.0f, 0.0f, 0.0f)); // Cor vermelha para o rastro

    // Tocar som
    if (GetGame()->GetAudio())
    {
        GetGame()->GetAudio()->PlaySound("Glitch.flac");
    }

    SDL_Log("BOSS launched missile!");
}

bool Boss::CanSeePlayer()
{
    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return false;

    Vector2 playerPos = player->GetPosition();
    float distance = (playerPos - mPosition).Length();

    return distance <= DETECTION_RANGE;
}

void Boss::TakeDamage(int damage)
{
    if (mIsDying)
        return;

    mHitPoints -= damage;
    SDL_Log("BOSS took %d damage! HP: %d/%d", damage, mHitPoints, mMaxHitPoints);

    if (mHitPoints <= 0)
    {
        mHitPoints = 0;
        Kill();
    }
    else
    {
        // Tocar som de dano
        if (GetGame()->GetAudio())
        {
            GetGame()->GetAudio()->PlaySound("hurt.wav");
        }
    }
}

void Boss::Kill()
{
    if (mIsDying)
        return;

    mIsDying = true;
    mAnimatorComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    // Tocar som de morte
    if (GetGame()->GetAudio())
    {
        GetGame()->GetAudio()->PlaySound("ShipDie.wav");
    }

    // Entrar em cena de vitória
    GetGame()->SetScene(GameScene::Win);

    SDL_Log("BOSS defeated! Triggering WIN scene.");
}

void Boss::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying)
        return;

    if (other->GetLayer() == ColliderLayer::Blocks)
    {
        // Inverter direção ao colidir com blocos
        mPatrolDirection *= -1;
    }
    else if (other->GetLayer() == ColliderLayer::Player)
    {
        // Causar dano ao jogador e empurrá-lo para trás
        Robot* player = GetGame()->GetPlayer();
        if (player)
        {
            player->TakeDamage(1);
            
            // Empurrar jogador na direção oposta ao Boss
            Vector2 pushDirection = player->GetPosition() - mPosition;
            if (pushDirection.x != 0)
            {
                pushDirection.x = pushDirection.x > 0 ? 1.0f : -1.0f;
            }
            
            SDL_Log("BOSS hit player!");
        }
        
        // Inverter direção do Boss para evitar travamento
        mPatrolDirection *= -1;
    }
}

void Boss::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying)
        return;

    if (other->GetLayer() == ColliderLayer::Player)
    {
        // Causar dano ao jogador
        Robot* player = GetGame()->GetPlayer();
        if (player)
        {
            player->TakeDamage(1);
            SDL_Log("BOSS hit player (vertical)!");
        }
        
        // Inverter direção do Boss para evitar travamento quando o jogador pula em cima
        mPatrolDirection *= -1;
    }
}

void Boss::OnProcessInput(const Uint8* keyState)
{
    // O boss não responde a inputs do jogador
}
