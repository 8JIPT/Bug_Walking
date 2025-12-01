//
// Created by pesso
//

#pragma once

#include "UIScreen.h"

class Win : public UIScreen
{
public:
    Win(class Game* game, const std::string& fontName);

    void HandleKeyPress(int key) override;
};
