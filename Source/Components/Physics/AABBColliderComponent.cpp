//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AABBColliderComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"

AABBColliderComponent::AABBColliderComponent(class Actor* owner, int dx, int dy, int w, int h,
        ColliderLayer layer, bool isStatic, int updateOrder)
        :Component(owner, updateOrder)
        ,mOffset(Vector2((float)dx, (float)dy))
        ,mIsStatic(isStatic)
        ,mWidth(w)
        ,mHeight(h)
        ,mLayer(layer)
{
    GetGame()->AddCollider(this);
}

AABBColliderComponent::~AABBColliderComponent()
{
    GetGame()->RemoveCollider(this);
}

Vector2 AABBColliderComponent::GetMin() const
{
    return mOwner->GetPosition() + mOffset - Vector2(mWidth / 2.0f, mHeight / 2.0f);
}

Vector2 AABBColliderComponent::GetMax() const
{
    return mOwner->GetPosition() + mOffset + Vector2(mWidth / 2.0f, mHeight / 2.0f);
}

bool AABBColliderComponent::Intersect(const AABBColliderComponent& b) const
{
    Vector2 minA = GetMin();
    Vector2 maxA = GetMax();
    Vector2 minB = b.GetMin();
    Vector2 maxB = b.GetMax();

    bool overlapX = maxA.x > minB.x && minA.x < maxB.x;
    bool overlapY = maxA.y > minB.y && minA.y < maxB.y;

    return overlapX && overlapY;
}

float AABBColliderComponent::GetMinVerticalOverlap(AABBColliderComponent* b) const
{
    Vector2 minA = GetMin();
    Vector2 maxA = GetMax();
    Vector2 minB = b->GetMin();
    Vector2 maxB = b->GetMax();

    float overlapDown = maxA.y - minB.y;
    float overlapUp = maxB.y - minA.y;

    if (overlapDown < overlapUp){
        return overlapDown;
    } else{
        return -overlapUp;
    }
}

float AABBColliderComponent::GetMinHorizontalOverlap(AABBColliderComponent* b) const
{
    Vector2 minA = GetMin();
    Vector2 maxA = GetMax();
    Vector2 minB = b->GetMin();
    Vector2 maxB = b->GetMax();

    float overlapRight = maxA.x - minB.x;
    float overlapLeft = maxB.x - minA.x;

    if (overlapRight < overlapLeft){
        return overlapRight;
    } else{
        return -overlapLeft;
    }
}

float AABBColliderComponent::DetectHorizontalCollision(RigidBodyComponent *rigidBody)
{
    if (mIsStatic) return 0.0f;

    float overlap = 0.0f;
    auto colliders = GetGame()->GetColliders();
    for (auto collider : colliders){
        if (collider == this || !collider->IsEnabled() || !Intersect(*collider)) continue;
        
        // Skip collision if both colliders belong to the same actor (self-collision)
        if (mOwner == collider->GetOwner()) continue;

        float minOverlap = GetMinHorizontalOverlap(collider);

        if (collider->GetLayer() == ColliderLayer::Blocks){
            ResolveHorizontalCollisions(rigidBody, minOverlap);
            mOwner->OnHorizontalCollision(minOverlap, collider);
            overlap = minOverlap;
            break;
        }
        if ((mLayer == ColliderLayer::Enemy && collider->GetLayer() == ColliderLayer::Player) || (mLayer == ColliderLayer::Player && collider->GetLayer() == ColliderLayer::Enemy)){
            AABBColliderComponent* playerCollider = (mLayer == ColliderLayer::Player) ? this : collider;
            AABBColliderComponent* enemyCollider  = (mLayer == ColliderLayer::Enemy)  ? this : collider;

            // Check if player is coming from above (stomping) - if so, skip horizontal collision damage
            Vector2 playerMin = playerCollider->GetMin();
            Vector2 playerMax = playerCollider->GetMax();
            Vector2 enemyMin = enemyCollider->GetMin();
            Vector2 enemyMax = enemyCollider->GetMax();
            
            Actor* enemyActor = enemyCollider->GetOwner();
            Actor* playerActor = playerCollider->GetOwner();
            
            if (enemyActor && playerActor) {
                // Get actual positions
                Vector2 playerPos = playerActor->GetPosition();
                Vector2 enemyPos = enemyActor->GetPosition();
                
                // Check if player is falling (attempting stomp)
                RigidBodyComponent* playerRB = playerActor->GetComponent<RigidBodyComponent>();
                bool isFalling = playerRB && playerRB->GetVelocity().y > 0.0f;
                
                // If player is above enemy OR falling (stomping attempt), skip horizontal damage
                if (playerPos.y < enemyPos.y - 10.0f || isFalling) {
                    continue;
                }
            }

            if (playerCollider->GetOwner()){
                playerCollider->GetOwner()->OnHorizontalCollision(minOverlap, enemyCollider);
            }
            overlap = minOverlap;
            break;
        }

        if ((mLayer == ColliderLayer::Item && collider->GetLayer() == ColliderLayer::Player) || (mLayer == ColliderLayer::Player && collider->GetLayer() == ColliderLayer::Item)){
            AABBColliderComponent* playerCollider = (mLayer == ColliderLayer::Player) ? this : collider;
            AABBColliderComponent* itemCollider   = (mLayer == ColliderLayer::Item) ? this : collider;

            if (playerCollider->GetOwner()){
                playerCollider->GetOwner()->OnHorizontalCollision(minOverlap, itemCollider);
            }
            if (itemCollider->GetOwner()){
                itemCollider->GetOwner()->OnHorizontalCollision(minOverlap, playerCollider);
            }
            overlap = minOverlap;
            break;
        }

        // Enemy-Enemy
        if (mLayer == ColliderLayer::Enemy && collider->GetLayer() == ColliderLayer::Enemy){
            // Only resolve collision for walkers
            if (mOwner && strcmp(mOwner->GetName(), "Walker") == 0){
                Actor* otherActor = collider->GetOwner();
                if (otherActor && strcmp(otherActor->GetName(), "Walker") == 0){
                    ResolveHorizontalCollisions(rigidBody, minOverlap);
                    mOwner->OnHorizontalCollision(minOverlap, collider);
                    overlap = minOverlap;
                    break;
                }
            }
        }
    }

    return overlap;
}

float AABBColliderComponent::DetectVertialCollision(RigidBodyComponent *rigidBody)
{
    if (mIsStatic) return 0.0f;

    float overlap = 0.0f;
    auto colliders = GetGame()->GetColliders();

    for (auto collider : colliders){
        if (collider == this || !collider->IsEnabled() || !Intersect(*collider)) continue;
        
        // Skip collision if both colliders belong to the same actor (self-collision)
        if (mOwner == collider->GetOwner()) continue;

        float minOverlap = GetMinVerticalOverlap(collider);
        if (collider->GetLayer() == ColliderLayer::Blocks){
            ResolveVerticalCollisions(rigidBody, minOverlap);
            mOwner->OnVerticalCollision(minOverlap, collider);
            overlap = minOverlap;
            break;
        }
        if ((mLayer == ColliderLayer::Enemy && collider->GetLayer() == ColliderLayer::Player) || (mLayer == ColliderLayer::Player && collider->GetLayer() == ColliderLayer::Enemy)){
            AABBColliderComponent* playerCollider = (mLayer == ColliderLayer::Player) ? this : collider;
            AABBColliderComponent* enemyCollider  = (mLayer == ColliderLayer::Enemy)  ? this : collider;
            if (playerCollider->GetOwner()){
                playerCollider->GetOwner()->OnVerticalCollision(minOverlap, enemyCollider);
            }
            overlap = minOverlap;
            break;
        }

        if ((mLayer == ColliderLayer::Item && collider->GetLayer() == ColliderLayer::Player) || (mLayer == ColliderLayer::Player && collider->GetLayer() == ColliderLayer::Item)){
            AABBColliderComponent* playerCollider = (mLayer == ColliderLayer::Player) ? this : collider;
            AABBColliderComponent* itemCollider   = (mLayer == ColliderLayer::Item) ? this : collider;

            if (playerCollider->GetOwner()){
                playerCollider->GetOwner()->OnVerticalCollision(minOverlap, itemCollider);
            }
            if (itemCollider->GetOwner()){
                itemCollider->GetOwner()->OnVerticalCollision(minOverlap, playerCollider);
            }
            overlap = minOverlap;
            break;
        }
    }

    return overlap;
}

void AABBColliderComponent::ResolveHorizontalCollisions(RigidBodyComponent *rigidBody, const float minXOverlap)
{
    Vector2 pos = mOwner->GetPosition();
    pos.x -= minXOverlap;
    mOwner->SetPosition(pos);
}

void AABBColliderComponent::ResolveVerticalCollisions(RigidBodyComponent *rigidBody, const float minYOverlap)
{
    Vector2 pos = mOwner->GetPosition();
    pos.y -= minYOverlap;
    mOwner->SetPosition(pos);

    rigidBody->SetVelocity(Vector2(rigidBody->GetVelocity().x, 0.0f));

    if (minYOverlap > 0){
        mOwner->SetOnGround();
    }
}

void AABBColliderComponent::DebugDraw(class Renderer *renderer)
{
    renderer->DrawRect(mOwner->GetPosition() + mOffset,Vector2((float)mWidth, (float)mHeight), mOwner->GetRotation(),
                       Color::Green, mOwner->GetGame()->GetCameraPos(), RendererMode::LINES);
}