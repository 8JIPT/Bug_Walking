//
// Created by pesso
//

#include "ChaserSpawner.h"
#include "../Game.h"
#include "Robot.h"
#include "Chaser.h"

ChaserSpawner::ChaserSpawner(Game* game, float spawnDistance)
    : Actor(game)
    , mSpawnDistance(spawnDistance)
{
}

void ChaserSpawner::OnUpdate(float deltaTime)
{
    if (mState == ActorState::Destroy)
        return;

    Robot* player = GetGame()->GetPlayer();
    if (!player)
        return;

    float dx = Math::Abs(player->GetPosition().x - GetPosition().x);
    if (dx <= mSpawnDistance)
    {
        Chaser* chaser = new Chaser(GetGame());
        chaser->SetPosition(GetPosition());
        SDL_Log("Spawned Chaser at (%.2f, %.2f)", chaser->GetPosition().x, chaser->GetPosition().y);
        mState = ActorState::Destroy;
    }
}
