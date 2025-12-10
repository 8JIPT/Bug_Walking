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
    , mBossHealthBarVisible(false)
    , mMaxBossBarWidth(300.0f)
{
    Vector2 hudPos(80.0f, 40.0f);
    int heartSpacing = 40;

    mHeart1 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x, hudPos.y), 2.0f);
    mHeart2 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x + heartSpacing, hudPos.y), 2.0f);
    mHeart3 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x + heartSpacing * 2, hudPos.y), 2.0f);
    mHeart4 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x + heartSpacing * 3, hudPos.y), 2.0f);
    mHeart5 = AddImage("../Assets/HUD/heart1.png", Vector2(hudPos.x + heartSpacing * 4, hudPos.y), 2.0f);

    // Barra de conserto
    Vector2 gaugePos(120.0f, 100.0f);
    mRepairGauge = new RepairGauge(game, gaugePos);
    
    // Boss health bar (no topo da tela, centralizado)
    Vector2 bossBarPos(Game::WINDOW_WIDTH / 2.0f, 50.0f);
    float barWidth = 300.0f;
    float barHeight = 20.0f;
    float borderThickness = 3.0f;

    // Fundo da barra (vermelho escuro)
    mBossHealthBarBg = AddRect(bossBarPos, Vector2(barWidth, barHeight));
    mBossHealthBarBg->SetColor(Vector4(0.4f, 0.0f, 0.0f, 1.0f));
    mBossHealthBarBg->SetIsVisible(false);
    
    // Barra de vida (vermelho)
    mBossHealthBar = AddRect(bossBarPos, Vector2(barWidth, barHeight));
    mBossHealthBar->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    mBossHealthBar->SetIsVisible(false);

    // Bordas
    Vector2 topPos = bossBarPos + Vector2(0.0f, -barHeight / 2.0f - borderThickness / 2.0f);
    Vector2 bottomPos = bossBarPos + Vector2(0.0f, barHeight / 2.0f + borderThickness / 2.0f);
    Vector2 leftPos = bossBarPos + Vector2(-barWidth / 2.0f - borderThickness / 2.0f, 0.0f);
    Vector2 rightPos = bossBarPos + Vector2(barWidth / 2.0f + borderThickness / 2.0f, 0.0f);

    mBossBorderTop = AddRect(topPos, Vector2(barWidth + borderThickness * 2.0f, borderThickness));
    mBossBorderBottom = AddRect(bottomPos, Vector2(barWidth + borderThickness * 2.0f, borderThickness));
    mBossBorderLeft = AddRect(leftPos, Vector2(borderThickness, barHeight + borderThickness * 2.0f));
    mBossBorderRight = AddRect(rightPos, Vector2(borderThickness, barHeight + borderThickness * 2.0f));

    Vector4 borderColor = Vector4(0.9f, 0.9f, 0.9f, 1.0f);
    mBossBorderTop->SetColor(borderColor);
    mBossBorderBottom->SetColor(borderColor);
    mBossBorderLeft->SetColor(borderColor);
    mBossBorderRight->SetColor(borderColor);

    mBossBorderTop->SetIsVisible(false);
    mBossBorderBottom->SetIsVisible(false);
    mBossBorderLeft->SetIsVisible(false);
    mBossBorderRight->SetIsVisible(false);
}

HUD::~HUD()
{
    if (mRepairGauge)
    {
        delete mRepairGauge;
        mRepairGauge = nullptr;
    }
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

void HUD::SetBossHealth(int health, int maxHealth)
{
    if (!mBossHealthBarVisible) return;
    
    float percentage = static_cast<float>(health) / static_cast<float>(maxHealth);
    float currentWidth = mMaxBossBarWidth * percentage;
    
    mBossHealthBar->SetSize(Vector2(currentWidth, 20.0f));
    
    Vector2 centerPos(Game::WINDOW_WIDTH / 2.0f, 50.0f);
    float offset = (mMaxBossBarWidth - currentWidth) / 2.0f;
    mBossHealthBar->SetOffset(Vector2(centerPos.x - offset, centerPos.y));

    // Atualizar posições das bordas para manter centralização
    float barWidth = mMaxBossBarWidth;
    float barHeight = 20.0f;
    float borderThickness = 3.0f;

    Vector2 topPos = centerPos + Vector2(0.0f, -barHeight / 2.0f - borderThickness / 2.0f);
    Vector2 bottomPos = centerPos + Vector2(0.0f, barHeight / 2.0f + borderThickness / 2.0f);
    Vector2 leftPos = centerPos + Vector2(-barWidth / 2.0f - borderThickness / 2.0f, 0.0f);
    Vector2 rightPos = centerPos + Vector2(barWidth / 2.0f + borderThickness / 2.0f, 0.0f);

    if (mBossBorderTop) mBossBorderTop->SetOffset(topPos);
    if (mBossBorderBottom) mBossBorderBottom->SetOffset(bottomPos);
    if (mBossBorderLeft) mBossBorderLeft->SetOffset(leftPos);
    if (mBossBorderRight) mBossBorderRight->SetOffset(rightPos);
}

void HUD::ShowBossHealthBar(bool show)
{
    mBossHealthBarVisible = show;
    mBossHealthBarBg->SetIsVisible(show);
    mBossHealthBar->SetIsVisible(show);
    if (mBossBorderTop) mBossBorderTop->SetIsVisible(show);
    if (mBossBorderBottom) mBossBorderBottom->SetIsVisible(show);
    if (mBossBorderLeft) mBossBorderLeft->SetIsVisible(show);
    if (mBossBorderRight) mBossBorderRight->SetIsVisible(show);
}

void HUD::UpdateRepairState(RepairLevel level)
{
    if (mRepairGauge) {
        mRepairGauge->UpdateState(level);
    }
}
