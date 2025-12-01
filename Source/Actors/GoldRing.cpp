#include "GoldRing.h"
#include "Robot.h" // Adicionado para acessar Robot
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

GoldRing::GoldRing(Game* game)
    : Actor(game)
{
    // Cria animação do anel dourado
    auto* anim = new AnimatorComponent(this, "../Assets/Sprites/GoldRing/GoldRing_Animation_RP1 (16x16).png", "../Assets/Sprites/GoldRing/GoldRing.json", 16, 16, 200);
    anim->AddAnimation("spin", {0, 1, 2, 3});
    anim->SetAnimFPS(10.0f);
    anim->SetAnimation("spin");
    // Collider para coleta
    new AABBColliderComponent(this, 0, 0, 16, 16, ColliderLayer::Item, true);
}

void GoldRing::OnUpdate(float deltaTime) {
    // Pode adicionar lógica de animação ou efeito aqui
}

void GoldRing::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    SDL_Log("[GoldRing] OnHorizontalCollision chamado");
    if (other && other->GetOwner()) {
        SDL_Log("[GoldRing] Colidiu com: %s", other->GetOwner()->GetName());
        if (strcmp(other->GetOwner()->GetName(), "Robot") == 0) {
            Robot* robot = dynamic_cast<Robot*>(other->GetOwner());
            if (robot) {
                SDL_Log("[GoldRing] Cast para Robot* OK, chamando UpgradeRepairLevel");
                robot->UpgradeRepairLevel();
            } else {
                SDL_Log("[GoldRing] Cast para Robot* FALHOU");
            }
            SDL_Log("[GoldRing] Chamando SetState(ActorState::Destroy)");
            SetState(ActorState::Destroy);
        }
    }
}

void GoldRing::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {
    SDL_Log("[GoldRing] OnVerticalCollision chamado");
    if (other && other->GetOwner()) {
        SDL_Log("[GoldRing] Colidiu com: %s", other->GetOwner()->GetName());
        if (strcmp(other->GetOwner()->GetName(), "Robot") == 0) {
            Robot* robot = dynamic_cast<Robot*>(other->GetOwner());
            if (robot) {
                SDL_Log("[GoldRing] Cast para Robot* OK, chamando UpgradeRepairLevel");
                robot->UpgradeRepairLevel();
            } else {
                SDL_Log("[GoldRing] Cast para Robot* FALHOU");
            }
            SDL_Log("[GoldRing] Chamando SetState(ActorState::Destroy)");
            SetState(ActorState::Destroy);
        }
    }
}
