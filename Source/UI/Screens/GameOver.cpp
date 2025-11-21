//
// Created by Lucas N. Ferreira on 06/11/25.
//

#include "GameOver.h"
#include "../../Game.h"
#include "CrossFadeScreen.h"

GameOver::GameOver(class Game* game, const std::string& fontName)
        :UIScreen(game, fontName)
{
        AddText("Game Over", Vector2(0.0f, 50.0f), 2.0f);
        AddButton("Press Enter",[this](){ this->Close(); new CrossFadeScreen(mGame, GameScene::MainMenu); },Vector2(0.0f, -100.0f),1.5f);
}

void GameOver::HandleKeyPress(int key)
{
        if (key == SDLK_RETURN && !mButtons.empty()){
                mButtons[mSelectedButtonIndex]->OnClick();
        }
}