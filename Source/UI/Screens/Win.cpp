//
// Created by pesso
//

#include "Win.h"
#include "../../Game.h"
#include "CrossFadeScreen.h"

Win::Win(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        float centerX = mGame->WINDOW_WIDTH / 2.0f;
        float centerY = mGame->WINDOW_HEIGHT / 2.0f;
        AddText("You Won This Level!", Vector2(centerX, centerY - 150.0f), 2.0f, 0.0f, 48);

        //background
        AddImage("../Assets/Background.png", Vector2(centerX, centerY - 170.0f), 4.4f, 0.0f, 10);

        // Next Level button
        UIButton* nextLevelButton = AddButton("Next Level", [this]() {
                int currentLevel = mGame->LoadProgressData();
                int nextLevel = currentLevel + 1;
                
                // Check if next level exists
                if (mGame->CheckLevelExists(nextLevel)) {
                        mGame->SaveProgressData(nextLevel);
                        this->Close();
                        new CrossFadeScreen(mGame, GameScene::Level);
                } else {
                        SDL_Log("No more levels available. Returning to main menu.");
                        this->Close();
                        new CrossFadeScreen(mGame, GameScene::MainMenu);
                }
        }, Vector2(centerX, centerY), 1.0f, 0.0f, 32, 300, 110);
        nextLevelButton->SetTextColor(Color::White);

        // Quit button
        UIButton* quitButton = AddButton("Quit Game", [game = mGame]() {
                game->Quit();
        }, Vector2(centerX, centerY + 100.0f), 1.0f, 0.0f, 32, 300, 110);
        quitButton->SetTextColor(Color::White);
}

void Win::HandleKeyPress(int key)
{
        if (mButtons.empty()) return;
        if (key == SDLK_DOWN) {
                mGame->GetAudio()->PlaySound("Minimalist4.wav");
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        } else if (key == SDLK_UP) {
                mGame->GetAudio()->PlaySound("Minimalist4.wav");
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex = (mSelectedButtonIndex - 1 + mButtons.size()) % mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                mGame->GetAudio()->PlaySound("Retro9.wav");
                mButtons[mSelectedButtonIndex]->OnClick();
        }
}
