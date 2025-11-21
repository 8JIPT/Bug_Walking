//
// Created by pesso on 17/11/2025.
//

#include "CrossFadeScreen.h"
#include "../../Math.h"
#include "../UIRect.h"

CrossFadeScreen::CrossFadeScreen(Game* game, GameScene nextScene)
    : UIScreen(game, "../Assets/Fonts/Arial.ttf")
    , mAlpha(0.0f)
    , mFadeSpeed(2.0f)
    , mState(FadeState::FadeOut)
    , mNextScene(nextScene)
{
    mGame->SetSceneTransitioning(true);
    
    // Position at center of screen with size covering the entire screen
    float centerX = Game::WINDOW_WIDTH / 2.0f;
    float centerY = Game::WINDOW_HEIGHT / 2.0f;
    Vector2 screenSize(Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT);

    mFadeRect = AddRect(Vector2(centerX, centerY), screenSize, 1.0f, 0.0f, 5000);
    mFadeRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
}

void CrossFadeScreen::Update(float deltaTime){
    if (mState == FadeState::Done) return;

    switch (mState){
        case FadeState::FadeOut:
            mAlpha += mFadeSpeed * deltaTime;
            if (mAlpha >= 1.0f){
                mAlpha = 1.0f;
                mFadeRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, mAlpha));
                mState = FadeState::LoadNewScene;
            }else{
                mFadeRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, mAlpha));
            }
            break;
        case FadeState::LoadNewScene:
            mGame->SetScene(mNextScene);
            mState = FadeState::FadeIn;
            break;
        case FadeState::FadeIn:
            mAlpha -= mFadeSpeed * deltaTime;
            if (mAlpha <= 0.0f){
                mAlpha = 0.0f;
                mFadeRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, mAlpha));
                mState = FadeState::Done;
                Close();
            }else{
                mFadeRect->SetColor(Vector4(0.0f, 0.0f, 0.0f, mAlpha));
            }
            break;
        case FadeState::Done:
            mGame->SetSceneTransitioning(false);
            break;
    }
}