#pragma once
#include "Actor.h"

class SmokeEffect : public Actor
{
public:
    SmokeEffect(class Game* game, const Vector2& position, float direction);

    void OnUpdate(float deltaTime) override;

private:
    class AnimatorComponent* mAnim;
    float mLifeTime;
};