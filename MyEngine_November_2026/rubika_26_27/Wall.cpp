#include "Wall.h"

#include <ranges>

#include "RenderComponent.h"
#include "SpriteDrawable.h"
#include "TransformComponent.h"
#include "DynamicColliderComponent.h"
#include "RectangleDrawable.h"

Wall::Wall(std::string friendlyName,  std::string tileName, sf::Vector2f pos) 
    : Entity(std::move(friendlyName)), tileName(std::move(tileName))
{
    Tags = TAG::NEUTRAL | TAG::STATIC;
    
    auto* tc = AddComponent<TransformComponent>();
    AddComponent<RenderComponent>();
    AddComponent<DynamicColliderComponent>();
    tc->SetWorldPosition(pos);
}
Wall::~Wall() 
{}

void Wall::Start()
{
    const char* texturePath = "../../MyEngine/Ressources/Basement.png";
    
    RenderComponent* rc = GetComponent<RenderComponent>();
    {
        RectangleDrawable* cornerWall = rc->AddNewDrawable<RectangleDrawable>("WallCorner", sf::Vector2f(0, 0), 0, sf::Vector2f(1, 1));
        cornerWall->SetVisibility(true);
        cornerWall->SetTexture(texturePath);
        cornerWall->SetTile(tileName);
    }
    
    auto* dcc = GetComponent<DynamicColliderComponent>();
    {
        dcc->SetHitboxSize({52, 52});
        dcc->SetHitboxOffset({0, 0});
    }
    
    Entity::Start();
}

void Wall::SetPosition(sf::Vector2f pos)
{
    TransformComponent* tc = GetComponent<TransformComponent>();
    tc->SetWorldPosition(pos);
}