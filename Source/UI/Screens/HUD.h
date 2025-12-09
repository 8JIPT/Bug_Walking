//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <string>

#include "UIScreen.h"

class HUD : public UIScreen
{
public:
    HUD(class Game* game, const std::string& fontName);

    void SetHealth(int health);
    void SetScore(int score);
    void Update(float deltaTime) override;

private:
    void UpdateHeartAnimation(float deltaTime);

    // HUD elements
    UIImage* mHeart1;
    UIImage* mHeart2;
    UIImage* mHeart3;
    UIImage* mHeart4;
    UIImage* mHeart5;
    UIText  *mScore;
    
    int mCurrentHealth;
    int mPreviousHealth;
    float mHeartAnimTimer;
    bool mIsAnimatingHeart;
    int mAnimatingHeartIndex;
    int mCurrentHeartFrame;  // For animation: 0=full, 4=empty
};
