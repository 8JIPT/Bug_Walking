//
// Created by pesso on 17/11/2025.
//

#pragma once
#include "UIScreen.h"
#include "../../Game.h"

class CrossFadeScreen : public UIScreen
{
public:
    enum class FadeState { FadeOut, LoadNewScene, FadeIn, Done };

    CrossFadeScreen(class Game* game, GameScene nextScene);

    void Update(float deltaTime) override;

private:
    float mAlpha;
    float mFadeSpeed;
    FadeState mState;
    GameScene mNextScene;

    UIRect* mFadeRect;
};
