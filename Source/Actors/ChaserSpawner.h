//
// Created by pesso
//

#pragma once

#include "Actor.h"

class ChaserSpawner : public Actor
{
public:
    explicit ChaserSpawner(Game* game, float spawnDistance = 700.0f);
    const char* GetName() const override { return "ChaserSpawner"; }

    void OnUpdate(float deltaTime) override;

private:
    float mSpawnDistance;
};
