//
// Created by Lucas N. Ferreira on 08/12/23.
//

#include "HUD.h"
#include "../../Game.h"
#include <string>

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    // Position HUD in top-left corner with safe margins
    Vector2 hudPos(180.0f, 60.0f);
    mHealth3 = AddImage("../Assets/HUD/ShieldBlue.png",hudPos, 0.75f);
    mHealth2 = AddImage("../Assets/HUD/ShieldOrange.png",hudPos, 0.75f);
    mHealth1 = AddImage("../Assets/HUD/ShieldRed.png",hudPos, 0.75f);
    AddImage("../Assets/HUD/ShieldBar.png",hudPos, 0.75f);

    AddText("Score:", Vector2(hudPos.x, hudPos.y + 45.0f), 0.7f);
    mScore = AddText("0", Vector2(hudPos.x + 80.0f, hudPos.y + 45.0f), 0.7f);
}

void HUD::SetHealth(int health)
{
    mHealth1->SetIsVisible(health >= 1);
    mHealth2->SetIsVisible(health >= 2);
    mHealth3->SetIsVisible(health >= 3);
}

void HUD::SetScore(int score)
{
    mScore->SetText(std::to_string(score));
}
