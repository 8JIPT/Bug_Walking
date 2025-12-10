//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Block.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Block::Block(Game* game, const std::string &texturePath, bool hasCollision, float scale)
    :Actor(game)
    , mHasCollision(hasCollision)
    , mScale(scale)
{
    mDrawComponent = new AnimatorComponent(this, texturePath, "", mScale * Game::TILE_SIZE,mScale * Game::TILE_SIZE, 90);

    if (mHasCollision) new AABBColliderComponent(this, 0, 0, Game::TILE_SIZE, Game::TILE_SIZE, ColliderLayer::Blocks, true);

    mStartPos = GetPosition();
}

void Block::Kill(){

}

void Block::OnUpdate(float deltaTime){

}

void Block::SetFlip(bool flipH, bool flipV, bool flipD){
    if (flipH) {
        Vector2 scale = GetScale();
        scale.x *= -1.0f;
        SetScale(scale);
    }
    if (flipV) {
        Vector2 scale = GetScale();
        scale.y *= -1.0f;
        SetScale(scale);
    }
    if (flipD) {
        // Diagonal flip is equivalent to rotating 90 degrees counterclockwise
        SetRotation(GetRotation() + Math::PiOver2);
        Vector2 scale = GetScale();
        scale.x *= -1.0f;
        SetScale(scale);
    }
}
