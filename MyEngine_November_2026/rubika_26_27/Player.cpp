#include "Player.h"
#include "Globals.h"
#include "DynamicColliderComponent.h"
#include "PlayerComponent.h"
#include "RenderComponent.h"
#include "SpriteDrawable.h"
#include "TransformComponent.h"


Player::Player(std::string friendlyName) : Entity(friendlyName)
{
    Tags = TAG::PLAYER;
    
    AddComponent<TransformComponent>();
    AddComponent<RenderComponent>();
    AddComponent<PlayerComponent>();
    AddComponent<DynamicColliderComponent>();
}
Player::~Player() 
{}

void Player::Start()
{
    const char* texturePath = "../../MyEngine/Ressources/IsaacSprite.png";
    
    TransformComponent* tc = GetComponent<TransformComponent>();
    {
        tc->SetWorldPosition(sf::Vector2f(100.f, 100.f));
    }

    RenderComponent* rc = GetComponent<RenderComponent>();
    {
        SpriteDrawable* bodySprite = rc->AddNewDrawable<SpriteDrawable>("Body", sf::Vector2f(2, 0), 0, sf::Vector2f(1, 1));
        bodySprite->SetVisibility(true);
        bodySprite->SetTexture(texturePath);
        bodySprite->SetAnimation("Body_Vertical");

        SpriteDrawable* headSprite = rc->AddNewDrawable<SpriteDrawable>("Head", sf::Vector2f(-2.5f, -20), 0, sf::Vector2f(1, 1));
        headSprite->SetVisibility(true);
        headSprite->SetTexture(texturePath);
        headSprite->SetAnimation("Head_Down");
    }
    
    Entity::Start();
}
