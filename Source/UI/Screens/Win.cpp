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
        AddText("You Win!", Vector2(centerX, centerY - 150.0f), 2.0f, 0.0f, 48);

        //background
        AddImage("../Assets/Background.png", Vector2(centerX, centerY - 170.0f), 4.4f, 0.0f, 10);

        // Main Menu button
        UIButton* mainMenuButton = AddButton("Main Menu", [this]() {
                this->Close();
                new CrossFadeScreen(mGame, GameScene::MainMenu);
        }, Vector2(centerX, centerY), 1.0f, 0.0f, 32, 300, 110);
        mainMenuButton->SetTextColor(Color::White);

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
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        } else if (key == SDLK_UP) {
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex = (mSelectedButtonIndex - 1 + mButtons.size()) % mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
                mButtons[mSelectedButtonIndex]->OnClick();
        }
}
