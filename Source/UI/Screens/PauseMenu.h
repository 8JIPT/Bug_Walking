//
// Created by pesso on 16/11/2025.
//

#pragma once
#include "UIScreen.h"

class PauseMenu : public UIScreen
{
public:
    PauseMenu(class Game* game, const std::string& fontName);
    ~PauseMenu() override = default;

    void HandleKeyPress(int key) override;
};

