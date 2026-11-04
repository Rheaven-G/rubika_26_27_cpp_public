#pragma once
#define _DRAW_DEBUG_SHAPES true
#ifdef _DRAW_DEBUG_SHAPES
#include <SFML/Graphics/RectangleShape.hpp>
#endif

#include <IComponent.h>

#include <SfML/Graphics/Rect.hpp>
#include <functional>

#include "IDrawable.h"

class TransformComponent;

enum class OnCollisionChoice
{
	Ignore,
	Block,
};

class DynamicColliderComponent : public IComponent
{
public:
	DynamicColliderComponent(Entity& entity);
	~DynamicColliderComponent() override;

	virtual void Start() override;
	virtual void Update(float fDeltaTime) override;
	virtual void Destroy() override;

	void SetHitboxSize(sf::Vector2f size);
	void SetHitboxOffset(sf::Vector2f offset);

	void EnableCollision(bool bEnable);
	bool AreCollisionsEnabled() const;

	const sf::FloatRect& GetHitbox() const;

	void SetOnCollisionCallback(std::function<OnCollisionChoice(const Entity*)> callback);
	
	void UpdateTransform();
	void OnTransformUpdate();
	void RecalculateHitbox();
	
	const char* GetName() const override { return "DynamicColliderComponent"; }
	void DrawDebug() override;
	
	// Debug
	void Draw(sf::RenderWindow& windows);
	
private:
	TransformComponent* Transform;
	sf::Vector2f ColliderSize{16.f, 24.f};
	sf::Vector2f ColliderOffset{4.f, -16.f};
	sf::FloatRect Hitbox;
	bool bCollisionEnabled = true;
	
	std::function<void()>* ptrOnTransformUpdateCallback = 
		new std::function<void()>{[this](){this->OnTransformUpdate();}};
	std::vector<std::function<OnCollisionChoice(const Entity*)>> OnCollisionCallbacks;
	
#ifdef _DRAW_DEBUG_SHAPES
	sf::RectangleShape DebugShape;
#endif
};