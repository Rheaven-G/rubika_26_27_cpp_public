#include "DynamicColliderComponent.h"
#define DCC DynamicColliderComponent

#ifdef _DRAW_DEBUG_SHAPES
#include <SFML/Graphics/RenderStates.hpp>
#endif

#include "Entity.h"
#include "Logger.h"
#include "Profiler.h"
#include "TransformComponent.h"
#include "ImGui/imgui.h"


DCC::DynamicColliderComponent(Entity& entity) 
    : IComponent(entity), Transform(GetEntity().GetComponent<TransformComponent>())
{
}
DCC::~DynamicColliderComponent()=default;

void DCC::Start()
{
    // Transform = GetEntity().GetComponent<TransformComponent>();
    Hitbox = sf::FloatRect();
    {
        RecalculateHitbox(); //Hitbox.position
        Hitbox.size = ColliderSize;
    }
#ifdef _DRAW_DEBUG_SHAPES
    DebugShape.setPosition(Hitbox.position);
    DebugShape.setSize(Hitbox.size);
    DebugShape.setFillColor(sf::Color::Transparent);
    DebugShape.setOutlineColor(sf::Color::Green);
    DebugShape.setOutlineThickness(2);
#endif
    
    // Subscribe
    Transform->OnTransformUpdate.emplace_back(ptrOnTransformUpdateCallback);
};
void DCC::Update(float fDeltaTime) 
{}
void DCC::Destroy()
{
    // Unsubscribe
    auto& ls = Transform->OnTransformUpdate;
    for (size_t i {0}; i < ls.size(); i++)
    {
        if (ls[i] == ptrOnTransformUpdateCallback)
        {
            ls.erase(ls.begin() + i);
            break;
        }
    }
    delete ptrOnTransformUpdateCallback;
    ptrOnTransformUpdateCallback = nullptr;
}

void DCC::SetHitboxSize(sf::Vector2f size)
{
    ColliderSize = size;
}
void DCC::SetHitboxOffset(sf::Vector2f offset)
{
    ColliderOffset = offset;
}

void DCC::EnableCollision(bool bEnable) { bCollisionEnabled = bEnable; }
bool DCC::AreCollisionsEnabled() const { return bCollisionEnabled; }

const sf::FloatRect& DCC::GetHitbox() const
{
    return Hitbox;
}

void DCC::SetOnCollisionCallback(std::function<OnCollisionChoice(const Entity*)> callback)
{
    OnCollisionCallbacks.emplace_back(callback);
}

void DCC::UpdateTransform()
{}
void DCC::OnTransformUpdate()
{
    RecalculateHitbox();
}
void DCC::RecalculateHitbox()
{
    Hitbox.position = Transform->GetWorldPosition() + ColliderOffset;
    Hitbox.size = ColliderSize;
#ifdef _DRAW_DEBUG_SHAPES
    DebugShape.setPosition(Hitbox.position);
    DebugShape.setSize(Hitbox.size);
#endif
}

void DCC::DrawDebug()
{
#ifdef _USE_IMGUI
    if (ImGui::InputFloat2("Size", &ColliderSize.x))
        RecalculateHitbox();
    if (ImGui::InputFloat2("Offset", &ColliderOffset.x))
        RecalculateHitbox();
#endif
}

void DCC::Draw(sf::RenderWindow& window)
{
    sf::RenderStates states = sf::RenderStates::Default;

    window.draw(DebugShape, states);
}