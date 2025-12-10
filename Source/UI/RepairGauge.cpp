#include "RepairGauge.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Texture.h"
#include "../Game.h"
#include "../Renderer/Renderer.h"

RepairGauge::RepairGauge(Game* game, const Vector2& offset)
    : UIImage(game, "../Assets/HUD/RepairBar/RepairBar.png", offset)
    , mFrameIndex(5) // Começa no estado Crítico (Index 5)
{
    // Aumentamos a escala visual para ficar visível na tela (ex: 3x)
    mScale = 3.0f; 
}

void RepairGauge::UpdateState(RepairLevel level)
{
    // Sprites:
    // 100% - 1o sprite (Idx 0)
    // 2/3 - 3o sprite (Idx 2)
    // 1/3 - 6o sprite (Idx 5)

    switch (level) {
        case RepairLevel::Fixed:
            mFrameIndex = 0;
            break;
        case RepairLevel::Damaged:
            mFrameIndex = 2;
            break;
        case RepairLevel::Critical:
        default:
            mFrameIndex = 5;
            break;
    }
}

void RepairGauge::Draw(Shader* shader)
{
    if(!mTexture || !mIsVisible) return;

    Matrix4 scaleMat = Matrix4::CreateScale(FRAME_W * mScale,
                                            FRAME_H * mScale, 
                                            1.0f);

    Matrix4 rotMat = Matrix4::CreateRotationZ(mAngle);
    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mOffset.x, mOffset.y, 0.0f));
    Matrix4 world = scaleMat * rotMat * transMat;

    shader->SetMatrixUniform("uWorldTransform", world);

    // Recorte
    float uvWidth  = FRAME_W / TEXTURE_W; // larg normalizada
    float uvHeight = FRAME_H / TEXTURE_H; // alt normalizada
    
    float u = mFrameIndex * uvWidth; 
    float v = 0.0f;

    shader->SetVectorUniform("uTexRect", Vector4(u, v, uvWidth, uvHeight));

    shader->SetVectorUniform("uColor", Vector3::One);
    shader->SetFloatUniform("uTextureFactor", 1.0f);

    mTexture->SetActive();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}