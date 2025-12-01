#include "SmokeEffect.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"

SmokeEffect::SmokeEffect(Game* game, const Vector2& position, float direction)
    : Actor(game)
    , mLifeTime(0.0f)
{
    SetPosition(position);

    SetScale(Vector2(direction, 1.0f));

    mAnim = new AnimatorComponent(this,
                                  "../Assets/Sprites/Puff/Puff.png",
                                  "../Assets/Sprites/Puff/Puff.json",
                                  20, 20, 120);

    std::vector<int> puffFrames = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    mAnim->AddAnimation("puff", puffFrames);

    float fps = 18.0f;
    mAnim->SetAnimFPS(fps);
    mAnim->SetAnimation("puff");

    mLifeTime = (float)puffFrames.size() / fps;
}

void SmokeEffect::OnUpdate(float deltaTime)
{
    mLifeTime -= deltaTime;

    if (mLifeTime <= 0.0f) {
        SetState(ActorState::Destroy);
    }
}