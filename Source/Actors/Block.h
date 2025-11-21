//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "Actor.h"

class Block : public Actor
{
public:
    explicit Block(Game* game, const std::string &texturePath, bool canBeBumped = false, bool canCreateMushroom = false);

    void Kill() override;
    void OnUpdate(float deltaTime);
    bool GetCanBeBumped() const {return mCanBeBumped;}
private:
    Vector2 mStartPos;
    bool mBumped;
    float mBumpOffset;
    float mBumpSpeed;
    float mMaxBump;
    bool mCanBeBumped;
    bool mCanCreateMushroom;

    class AnimatorComponent* mDrawComponent;
};
