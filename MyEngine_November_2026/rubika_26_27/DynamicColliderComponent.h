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
	enum FLAGS
	{
		NONE = 0,
		ENABLED = 1 << 0,
		TRIGGER = 1 << 1,
	};
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
	void OnCollision(const DynamicColliderComponent* collider);
	void OnTrigger(const DynamicColliderComponent* collider);
	
	void UpdateTransform(sf::Vector2f direction = {0.f, 0.f});
	void OnTransformUpdate();
	void RecalculateHitbox();
	
	const char* GetName() const override { return "DynamicColliderComponent"; }
	void DrawDebug() override;
	
	// Debug
	void Draw(sf::RenderWindow& windows);
	
	char Flags = FLAGS::ENABLED;
	
private:
	TransformComponent* Transform;
	sf::Vector2f ColliderSize{16.f, 24.f};
	sf::Vector2f ColliderOffset{4.f, -16.f};
	sf::FloatRect Hitbox;
	
	
	std::function<void()>* ptrOnTransformUpdateCallback = 
		new std::function<void()>{[this](){this->OnTransformUpdate();}};
	std::vector<std::function<OnCollisionChoice(const Entity*)>> OnCollisionCallbacks;
	
#ifdef _DRAW_DEBUG_SHAPES
	sf::RectangleShape DebugShape;
#endif
};