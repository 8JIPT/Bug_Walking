//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIButton.h"
#include "../Renderer/Texture.h"

UIButton::UIButton(class Game* game, std::function<void()> onClick, const std::string& text, class Font* font,
                   const Vector2 &offset, float scale, float angle, int pointSize, const unsigned wrapLength, int drawOrder)
        :UIText(game, text, font, offset, scale, angle, pointSize, wrapLength, drawOrder)
        ,mOnClick(onClick)
        ,mHighlighted(false)
{

}

UIButton::~UIButton()
{

}


void UIButton::OnClick()
{
    // Call attached handler, if it exists
    if (mOnClick) {
        mOnClick();
    }
}

void UIButton::Draw(class Shader* shader)
{
    // Store original background color
    Vector4 originalColor = mBackgroundColor;
    
    if(mHighlighted) {
        // Make background visible when highlighted (change color to yellow/gold)
        mBackgroundColor = Vector4(1.0f, 0.84f, 0.0f, 0.8f);
    }
    else {
        // Transparent background when not highlighted
        mBackgroundColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    UIText::Draw(shader);
    
    // Restore original color
    mBackgroundColor = originalColor;
}