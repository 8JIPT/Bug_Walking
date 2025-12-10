//
// Created by Lucas N. Ferreira on 08/12/23.
//

#pragma once

#include <string>
#include "UIScreen.h"
#include "../RepairGauge.h"

class HUD : public UIScreen
{
public:
    HUD(class Game* game, const std::string& fontName);
    ~HUD();

    void SetHealth(int health);
    void SetScore(int score);
    void SetBossHealth(int health, int maxHealth);
    void ShowBossHealthBar(bool show);
    void Update(float deltaTime) override;

    void UpdateRepairState(RepairLevel level);

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
    
    // Boss health bar
    UIRect* mBossHealthBarBg;
    UIRect* mBossHealthBar;
    // Borda da barra do boss (linhas)
    UIRect* mBossBorderTop;
    UIRect* mBossBorderBottom;
    UIRect* mBossBorderLeft;
    UIRect* mBossBorderRight;
    bool mBossHealthBarVisible;
    float mMaxBossBarWidth;

    RepairGauge* mRepairGauge;
};
