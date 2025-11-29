//
// Created by pesso
//

#include "WalkerSpawner.h"
#include "Walker.h"
#include "Mario.h"
#include "../Game.h"

WalkerSpawner::WalkerSpawner(Game* game, const Vector2& position)
    : Actor(game)
    , mHasSpawned(false)
{
    SetPosition(position);
}

void WalkerSpawner::OnUpdate(float deltaTime)
{
    if (mHasSpawned)
        return;

    Mario* player = GetGame()->GetPlayer();
    if (!player)
        return;

    Vector2 playerPos = player->GetPosition();
    float distance = (playerPos - mPosition).Length();
    if (distance <= Game::SPAWN_DISTANCE)
    {
        new Walker(GetGame(), mPosition);
        SDL_Log("Spawned Walker at (%.2f, %.2f)", mPosition.x, mPosition.y);
        mHasSpawned = true;

        mState = ActorState::Destroy;
    }
}
