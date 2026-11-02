#include "PlayerComponent.h"
#include "Globals.h"
#include "InputSystem.h"
#include "InputAction.h"
#include "ImGui/imgui.h"
#include "Entity.h"

PlayerComponent::PlayerComponent(Entity& entity) : IComponent(entity) {}
PlayerComponent::~PlayerComponent() = default;

void PlayerComponent::Start()
{
    Transform = GetEntity().GetComponent<TransformComponent>();
    
    // ---- BINDING to INPUTSYSTEM & INPUTACTIONS
    InputSystem& is = *gData.InputSystem;
    
    InputAction* MoveUP = is.FindAction("MoveUP");
    InputAction* MoveDOWN = is.FindAction("MoveDOWN");
    InputAction* MoveLEFT = is.FindAction("MoveLEFT");
    InputAction* MoveRIGHT = is.FindAction("MoveRIGHT");
    
    MoveUP->RegisterCallback("PlayerComponent::OnMoveX-", 
        InputAction::Moment::OnHold, 
        [this](float direction) {this->OnMoveY(direction); }, 
        this);
    MoveDOWN->RegisterCallback("PlayerComponent::OnMoveX+",
        InputAction::Moment::OnHold,
        [this](float direction) {this->OnMoveY(direction); }, 
        this);
    MoveLEFT->RegisterCallback("PlayerComponent::OnMoveY-", 
        InputAction::Moment::OnHold, 
        [this](float direction) {this->OnMoveX(direction); }, 
        this);
    MoveRIGHT->RegisterCallback("PlayerComponent::OnMoveY+",
        InputAction::Moment::OnHold,
        [this](float direction) {this->OnMoveX(direction); }, 
        this);
    // ---- END BINDING...
}
void PlayerComponent::Update(float fDeltaTime){}
void PlayerComponent::Destroy() {}

void PlayerComponent::DrawDebug()
{
#ifdef _USE_IMGUI
    ImGui::InputFloat("Movement Speed", &MovementSpeed); 
        
#endif
}


// Player
void PlayerComponent::OnMoveX(float val)
{
    sf::Vector2f position = Transform->GetWorldPosition();
    Transform->SetWorldPosition({position.x + val*MovementSpeed, position.y});
}
void PlayerComponent::OnMoveY(float val)
{
    sf::Vector2f position = Transform->GetWorldPosition();
    Transform->SetWorldPosition({position.x, position.y + val*MovementSpeed});
}
void PlayerComponent::OnShootX(float val) {}
void PlayerComponent::OnShootY(float val) {}