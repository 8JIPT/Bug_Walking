//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once
#include "../Component.h"
#include "../../Math.h"
#include "RigidBodyComponent.h"
#include <vector>
#include <set>

enum class ColliderLayer
{
    Player,
    Enemy,
    Blocks,
    Item
};

class AABBColliderComponent : public Component
{
public:

    AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
                                ColliderLayer layer, bool isStatic = false, int updateOrder = 10);
    ~AABBColliderComponent() override;

    bool Intersect(const AABBColliderComponent& b) const;

    float DetectHorizontalCollision(RigidBodyComponent *rigidBody);
    float DetectVertialCollision(RigidBodyComponent *rigidBody);

    Vector2 GetMin() const;
    Vector2 GetMax() const;
    ColliderLayer GetLayer() const { return mLayer; }

    // Drawing for debug purposes
    void DebugDraw(class Renderer* renderer) override;

    void SetSize(int width, int height) { mWidth = width; mHeight = height;}
    void SetOffset(int dx, int dy) { mOffset = Vector2((float)dx, (float)dy); }
    
    void SetTag(const char* tag) { mTag = tag; }
    const char* GetTag() const { return mTag; }

private:
    float GetMinVerticalOverlap(AABBColliderComponent* b) const;
    float GetMinHorizontalOverlap(AABBColliderComponent* b) const;

    void ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);
    void ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minOverlap);

    Vector2 mOffset;
    int mWidth;
    int mHeight;
    bool mIsStatic;

    ColliderLayer mLayer;
    const char* mTag = nullptr;
};