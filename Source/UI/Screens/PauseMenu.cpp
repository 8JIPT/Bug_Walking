//
// Created by pesso on 16/11/2025.
//

#include "PauseMenu.h"

#include "PauseMenu.h"
#include "../../Game.h"
#include "../../Actors/Mario.h"
#include "CrossFadeScreen.h"

PauseMenu::PauseMenu(class Game* game, const std::string& fontName)
    : UIScreen(game, fontName)
{
    float centerX = mGame->WINDOW_WIDTH / 2.0f;
    float centerY = mGame->WINDOW_HEIGHT / 2.0f;

    AddText("PAUSED", Vector2(centerX, centerY - 150.0f), 1.0f, 0.0f, 48);

    // resume button
    AddRect(Vector2(centerX, centerY), Vector2(300.f, 60.f), 1.0f, 0.0f, 90)->SetColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    UIButton* resumeButton = AddButton("Resume", [this]() {
        AudioSystem* audio = mGame->GetAudio();
        Mario* mario = mGame->GetPlayer();
        audio->ResumeSound(mGame->GetMusicHandle());
        mGame->SetIsPaused(false);
        this->Close();
    }, Vector2(centerX, centerY), 1.0f, 0.0f, 32, 300, 110);
    resumeButton->SetTextColor(Color::White);

    // Main Menu Button
    AddRect(Vector2(centerX, centerY + 100.0f), Vector2(300.f, 60.f), 1.0f, 0.0f, 90)->SetColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
    UIButton* menuButton = AddButton("Main Menu", [this]() {
        mGame->SetIsPaused(false);
        new CrossFadeScreen(mGame, GameScene::MainMenu);
    }, Vector2(centerX, centerY + 100.0f), 1.0f, 0.0f, 32, 300, 110);
    menuButton->SetTextColor(Color::White);
}

void PauseMenu::HandleKeyPress(int key){
    if (mButtons.empty()) return;
    if (key == SDLK_DOWN) {
        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }else if (key == SDLK_UP) {
        mButtons[mSelectedButtonIndex]->SetHighlighted(false);
        mSelectedButtonIndex = (mSelectedButtonIndex - 1 + mButtons.size()) % mButtons.size();
        mButtons[mSelectedButtonIndex]->SetHighlighted(true);
    }else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
        mButtons[mSelectedButtonIndex]->OnClick();
    }
}