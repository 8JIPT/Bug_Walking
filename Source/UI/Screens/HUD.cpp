//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "../../Game.h"
#include <string>

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
    , mCurrentHealth(5)
    , mPreviousHealth(5)
    , mHeartAnimTimer(0.0f)
    , mIsAnimatingHeart(false)
    , mAnimatingHeartIndex(-1)
    , mCurrentHeartFrame(0)
{
    Vector2 hudPos(80.0f, 40.0f);
    int heartSpacing = 40;

    mHeart1 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x, hudPos.y), 2.0f);
    mHeart2 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x + heartSpacing, hudPos.y), 2.0f);
    mHeart3 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x + heartSpacing * 2, hudPos.y), 2.0f);
    mHeart4 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x + heartSpacing * 3, hudPos.y), 2.0f);
    mHeart5 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x + heartSpacing * 4, hudPos.y), 2.0f);
}

void HUD::Update(float deltaTime)
{
    UIScreen::Update(deltaTime);
    UpdateHeartAnimation(deltaTime);
}

void HUD::UpdateHeartAnimation(float deltaTime)
{
    if (mIsAnimatingHeart)
    {
        mHeartAnimTimer += deltaTime;
        int targetFrame = static_cast<int>(mHeartAnimTimer * 10.0f);
        if (targetFrame != mCurrentHeartFrame && targetFrame <= 4)
        {
            mCurrentHeartFrame = targetFrame;
            std::string heartPath = "../Assets/HUD/heart" + std::to_string(mCurrentHeartFrame + 1) + ".png";
            
            UIImage* targetHeart = nullptr;
            if (mAnimatingHeartIndex == 0) targetHeart = mHeart1;
            else if (mAnimatingHeartIndex == 1) targetHeart = mHeart2;
            else if (mAnimatingHeartIndex == 2) targetHeart = mHeart3;
            else if (mAnimatingHeartIndex == 3) targetHeart = mHeart4;
            else if (mAnimatingHeartIndex == 4) targetHeart = mHeart5;
            
            if (targetHeart)
            {
                targetHeart->SetTexture(GetGame()->GetRenderer()->GetTexture(heartPath));
            }
        }
        if (mHeartAnimTimer >= 0.5f)
        {
            mIsAnimatingHeart = false;
            mHeartAnimTimer = 0.0f;
        }
    }
}

void HUD::SetHealth(int health)
{
    if (health < mCurrentHealth)
    {
        mIsAnimatingHeart = true;
        mHeartAnimTimer = 0.0f;
        mAnimatingHeartIndex = health;
        mCurrentHeartFrame = 0;
    }
    else if (health > mCurrentHealth)
    {
        if (health >= 1 && mHeart1)
        {
            mHeart1->SetTexture(GetGame()->GetRenderer()->GetTexture("../Assets/HUD/heart1.png"));
            mHeart1->SetIsVisible(true);
        }
        if (health >= 2 && mHeart2)
        {
            mHeart2->SetTexture(GetGame()->GetRenderer()->GetTexture("../Assets/HUD/heart1.png"));
            mHeart2->SetIsVisible(true);
        }
        if (health >= 3 && mHeart3)
        {
            mHeart3->SetTexture(GetGame()->GetRenderer()->GetTexture("../Assets/HUD/heart1.png"));
            mHeart3->SetIsVisible(true);
        }
        if (health >= 4 && mHeart4)
        {
            mHeart4->SetTexture(GetGame()->GetRenderer()->GetTexture("../Assets/HUD/heart1.png"));
            mHeart4->SetIsVisible(true);
        }
        if (health >= 5 && mHeart5)
        {
            mHeart5->SetTexture(GetGame()->GetRenderer()->GetTexture("../Assets/HUD/heart1.png"));
            mHeart5->SetIsVisible(true);
        }
    }
    
    mPreviousHealth = mCurrentHealth;
    mCurrentHealth = health;
}

void HUD::SetScore(int score)
{
    mScore->SetText(std::to_string(score));
}
