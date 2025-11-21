//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Mario.h"
#include "Goomba.h"

Spawner::Spawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{

}

void Spawner::OnUpdate(float deltaTime)
{
        if (mState == ActorState::Destroy) return;
        float dx = Math::Abs(GetGame()->GetPlayer()->GetPosition().x - GetPosition().x);
        if (dx <= mSpawnDistance){
                Goomba* g = new Goomba(GetGame());
                g->SetPosition(GetPosition());
                SDL_Log("Spawned Goomba at (%.2f, %.2f) state=%d", g->GetPosition().x, g->GetPosition().y, (int)g->GetState());

                mState = ActorState::Destroy;
        }
}