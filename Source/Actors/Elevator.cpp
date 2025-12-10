#include "Elevator.h"
#include "Robot.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"

Elevator::Elevator(Game* game)
    : Actor(game)
    , mGame(game)
    , mHasActivated(false)
    , mIsAnimating(false)
    , mActivationRadius(100.0f)
    , mAnimationTime(0.0f)
{
    mAnimator = new AnimatorComponent(this, "../Assets/Sprites/Elevator/elevator.png","../Assets/Sprites/Elevator/elevator.json",32, 48, 50);
    mAnimator->AddAnimation("activate", std::vector<int>{0, 1, 2, 3, 4, 5, 6});
    mAnimator->SetAnimFPS(8.0f);
    mAnimator->SetAnimation("activate");
    mAnimator->SetIsPaused(true);
}

Elevator::~Elevator()
{
}

void Elevator::OnUpdate(float deltaTime)
{
    if (mHasActivated)
    {
        return;
    }
    Robot* player = mGame->GetPlayer();
    if (!player)
    {
        return;
    }
    Vector2 toPlayer = player->GetPosition() - GetPosition();
    float distance = toPlayer.Length();
    if (distance <= mActivationRadius && !mIsAnimating)
    {
        mIsAnimating = true;
        mAnimator->SetIsPaused(false);
    }
    if (mIsAnimating)
    {
        mAnimationTime += deltaTime;
        if (mAnimationTime >= 0.75f)
        {
            mAnimator->SetIsPaused(true);
            mHasActivated = true;
        }
    }
}
