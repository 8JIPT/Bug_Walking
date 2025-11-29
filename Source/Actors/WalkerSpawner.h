//
// Created by pesso
//

#pragma once

#include "Actor.h"

class WalkerSpawner : public Actor
{
public:
    explicit WalkerSpawner(Game* game, const Vector2& position);
    const char* GetName() const override { return "WalkerSpawner"; }
    
    void OnUpdate(float deltaTime) override;

private:
    bool mHasSpawned;
};
