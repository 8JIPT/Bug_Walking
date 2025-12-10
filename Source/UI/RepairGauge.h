#pragma once
#include "UIImage.h"
#include "../Actors/Robot.h" // Para acessar o enum RepairLevel

class RepairGauge : public UIImage
{
public:
    RepairGauge(class Game* game, const Vector2& offset);
    void Draw(class Shader* shader) override;
    void UpdateState(RepairLevel level);

private:
    int mFrameIndex; // sprite p desenhar
    
    // cada sprite de barra é 3 blocos de 16x16
    const float TEXTURE_W = 384.0f;
    const float TEXTURE_H = 32.0f;
    const float FRAME_W   = 48.0f;
    const float FRAME_H   = 16.0f;
};