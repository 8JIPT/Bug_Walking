//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "../../Renderer/Texture.h"
#include <fstream>

AnimatorComponent::AnimatorComponent(class Actor* owner, const std::string &texPath, const std::string &dataPath,
                                     int width, int height, int drawOrder)
        :DrawComponent(owner,  drawOrder)
        ,mAnimTimer(0.0f)
        ,mIsPaused(false)
        ,mWidth(width)
        ,mHeight(height)
        ,mTextureFactor(1.0f)
{
    if (owner && owner->GetGame() && owner->GetGame()->GetRenderer()){
        mSpriteTexture = owner->GetGame()->GetRenderer()->GetTexture(texPath);
        if (!mSpriteTexture){
            SDL_Log("AnimatorComponent: falha ao carregar textura '%s'", texPath.c_str());
        }
    } else{
        SDL_Log("AnimatorComponent: owner ou renderer nulo");
    }

    if (!dataPath.empty()){
        if (!LoadSpriteSheetData(dataPath)){
            SDL_Log("AnimatorComponent: falha ao carregar sprite sheet data '%s'", dataPath.c_str());
        }
    }

}

AnimatorComponent::~AnimatorComponent()
{
    mAnimations.clear();
    mSpriteSheetData.clear();
}

bool AnimatorComponent::LoadSpriteSheetData(const std::string& dataPath)
{
    // Load sprite sheet data and return false if it fails
    std::ifstream spriteSheetFile(dataPath);

    if (!spriteSheetFile.is_open()) {
        SDL_Log("Failed to open sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    if (spriteSheetData.is_null()) {
        SDL_Log("Failed to parse sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    auto textureWidth = static_cast<float>(spriteSheetData["meta"]["size"]["w"].get<int>());
    auto textureHeight = static_cast<float>(spriteSheetData["meta"]["size"]["h"].get<int>());

    for(const auto& frame : spriteSheetData["frames"]) {

        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();

        mSpriteSheetData.emplace_back(static_cast<float>(x)/textureWidth, static_cast<float>(y)/textureHeight,
                                      static_cast<float>(w)/textureWidth, static_cast<float>(h)/textureHeight);
    }

    return true;
}

void AnimatorComponent::Draw(Renderer* renderer)
{
    if (!mIsVisible){
        return;
    }

    if (!mSpriteTexture){
        SDL_Log("AnimatorComponent::Draw - No texture!");
        return;
    }

    Vector2 position = mOwner->GetPosition() + mOffset;
    Vector2 scale = mOwner->GetScale();
    float rotation = mOwner->GetRotation();

    Vector2 size = Vector2(static_cast<float>(mWidth), static_cast<float>(mHeight));
    bool flip = false;
    if (scale.x < 0.0f){
        flip = true;
        size.x *= fabsf(scale.x);
    } else{
        size.x *= scale.x;
    }
    size.y *= scale.y;

    Vector4 texRect = Vector4::UnitRect;
    if (!mAnimName.empty()){
        auto it = mAnimations.find(mAnimName);
        if (it != mAnimations.end() && !it->second.empty() && !mSpriteSheetData.empty()){
            const std::vector<int>& frames = it->second;
            int numFrames = static_cast<int>(frames.size());
            if (numFrames > 0){
                int frameIndex = static_cast<int>(floorf(mAnimTimer)) % numFrames;
                if (frameIndex < 0) frameIndex += numFrames;
                int spriteIndex = frames[frameIndex];
                if (spriteIndex >= 0 && spriteIndex < static_cast<int>(mSpriteSheetData.size())){
                    texRect = mSpriteSheetData[spriteIndex];
                }
            }
        }
        else{
            if (!mSpriteSheetData.empty()) texRect = mSpriteSheetData[0];
        }
    } else{
        if (!mSpriteSheetData.empty()) texRect = mSpriteSheetData[0];
    }

    Vector3 color = Vector3::One;

    renderer->DrawTexture(position, size, rotation, color, mSpriteTexture, texRect, mOwner->GetGame()->GetCameraPos(), flip, mTextureFactor);

}

void AnimatorComponent::Update(float deltaTime)
{
    if (mIsPaused || mAnimations.empty()){
        return;
    }

    if (mAnimName.empty()){
        return;
    }

    auto it = mAnimations.find(mAnimName);
    if (it == mAnimations.end()){
        return;
    }

    const std::vector<int>& frames = it->second;
    if (frames.empty()) return;

    mAnimTimer += mAnimFPS * deltaTime;

    float frameCount = static_cast<float>(frames.size());
    if (mAnimTimer >= frameCount){
        mAnimTimer = fmodf(mAnimTimer, frameCount);
    } else if (mAnimTimer < 0.0f){
        mAnimTimer = fmodf(mAnimTimer, frameCount) + frameCount;
        if (mAnimTimer >= frameCount) mAnimTimer -= frameCount;
    }
}

void AnimatorComponent::SetAnimation(const std::string& name)
{
    if (mAnimName == name){
        return;
    }
    mAnimName = name;
    mAnimTimer = 0.0f;
    Update(0.0f);
}

void AnimatorComponent::AddAnimation(const std::string& name, const std::vector<int>& spriteNums)
{
    mAnimations.emplace(name, spriteNums);
}