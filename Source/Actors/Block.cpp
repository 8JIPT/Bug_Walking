//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Block.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "Mushroom.h"

Block::Block(Game* game, const std::string &texturePath, bool canBeBumped, bool canCreateMushroom)
    :Actor(game)
    , mBumped(false)
    , mBumpOffset(0.0f)
    , mBumpSpeed(100.0f)
    , mMaxBump(10.0f)
    , mCanBeBumped(canBeBumped)
    , mCanCreateMushroom(canCreateMushroom)
{
    mDrawComponent = new AnimatorComponent(this, texturePath, "", Game::TILE_SIZE, Game::TILE_SIZE);

    new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Blocks, true);

    mStartPos = GetPosition();
}

void Block::Kill(){
    if (!mBumped && mCanBeBumped){
        mBumped = true;
        mBumpOffset = 0.0f;
        mBumpSpeed = 100.0f;
        SDL_Log("Block Bumped!");
        if (mCanCreateMushroom){
            Vector2 spawnPos = GetPosition() + Vector2(0.0f, -static_cast<float>(Game::TILE_SIZE) - Game::EPS);
            new Mushroom(GetGame(), spawnPos, 150.0f);
            mCanCreateMushroom = false;
        }
    }
}

void Block::OnUpdate(float deltaTime){
    if (!mCanBeBumped || !mBumped) return;

    mBumpOffset += mBumpSpeed * deltaTime;

    if (mBumpOffset >= mMaxBump){
        mBumpOffset = mMaxBump;
        mBumpSpeed = -mBumpSpeed;
    } else if (mBumpOffset <= 0.0f){
        mBumpOffset = 0.0f;
        mBumpSpeed = Math::Abs(mBumpSpeed);
        mBumped = false;
    }

    mDrawComponent->SetOffset(Vector2(0.0f, -mBumpOffset));
}
