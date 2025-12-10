#pragma once
#include "Actor.h"

class Elevator : public Actor
{
public:
    Elevator(class Game* game);
    ~Elevator();
    
    const char* GetName() const override { return "Elevator"; }
    
    void OnUpdate(float deltaTime) override;
    
private:
    class AnimatorComponent* mAnimator;
    class Game* mGame;
    
    bool mHasActivated;
    bool mIsAnimating;
    float mActivationRadius;
    float mAnimationTime;
};
