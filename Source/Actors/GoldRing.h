#pragma once
#include "Actor.h"

class GoldRing : public Actor {
public:
    explicit GoldRing(Game* game);
    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    ~GoldRing() override {
        SDL_Log("GoldRing destruído!");
    }
    const char* GetName() const override { return "GoldRing"; }
};
