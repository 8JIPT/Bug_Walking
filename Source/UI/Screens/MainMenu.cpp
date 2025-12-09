//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "MainMenu.h"
#include "../../Game.h"
#include "CrossFadeScreen.h"

MainMenu::MainMenu(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        float centerX = mGame->WINDOW_WIDTH / 2.0f;
        float centerY = mGame->WINDOW_HEIGHT / 2.0f;

        //background
        AddImage("../Assets/Background.png", Vector2(centerX, centerY - 170.0f), 4.4f, 0.0f, 10);

        //logo
        AddImage("../Assets/Logo.png", Vector2(centerX, centerY - 170.0f), 0.5f, 0.0f, 100);

        //start button
        //AddRect(Vector2(centerX, centerY), Vector2(300.f, 60.f), 1.0f, 0.0f, 90)->SetColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        UIButton* startButton = AddButton("Start Game", [game = mGame, this](){
            this->Close();
            new CrossFadeScreen(game, GameScene::Level);
        }, Vector2(centerX, centerY), 1.0f, 0.0f, 32, 300, 110);
        startButton->SetTextColor(Color::White);

        //reset save button
        UIButton* resetButton = AddButton("Reset Save", [game = mGame](){
            game->SaveProgressData(1);
        }, Vector2(centerX, centerY + 100.0f), 1.0f, 0.0f, 32, 300, 110);
        resetButton->SetTextColor(Color::White);

        //quit button
        //AddRect(Vector2(centerX, centerY + 200.0f), Vector2(300.f, 60.f), 1.0f, 0.0f, 90)->SetColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f));
        UIButton* quitButton = AddButton("Quit",[game = mGame](){game->Quit();}, Vector2(centerX, centerY + 200.0f), 1.0f, 0.0f, 32, 300, 110);
        quitButton->SetTextColor(Color::White);
}

void MainMenu::HandleKeyPress(int key)
{
        if (mButtons.empty()) return;
        if (key == SDLK_DOWN){
                mGame->GetAudio()->PlaySound("Minimalist4.wav");
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex = (mSelectedButtonIndex + 1) % mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        } else if (key == SDLK_UP){
                mGame->GetAudio()->PlaySound("Minimalist4.wav");
                mButtons[mSelectedButtonIndex]->SetHighlighted(false);
                mSelectedButtonIndex = (mSelectedButtonIndex - 1 + mButtons.size()) % mButtons.size();
                mButtons[mSelectedButtonIndex]->SetHighlighted(true);
        } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER){
                mGame->GetAudio()->PlaySound("Retro9.wav");
                mButtons[mSelectedButtonIndex]->OnClick();
        }
}