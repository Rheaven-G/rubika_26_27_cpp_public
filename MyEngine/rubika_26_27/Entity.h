#pragma once

#include <string>
#include <vector>
#include "IComponent.h"
#include "RenderComponent.h"
#include "TransformComponent.h"

namespace sf
{
	class RenderWindow;
}

class Entity
{
public:
	std::string friendlyName;
	TransformComponent transform;
	// TODO: Change parent/children to TransformComponent domain
	Entity* parent = nullptr;
	std::vector<Entity*> children;
	
	Entity(std::string name, sf::Vector2f position);
	Entity& operator=(Entity const& other)=default;
	bool operator==(Entity const& other) const;
	virtual ~Entity();
	
	// ---- Getters
	std::string GetUUID();
	// ---- Setters
	template <typename C>
	C* AddComponent<C>();
	RenderComponent* AddComponent<RenderComponent>();
	
	virtual void Start();
	virtual void Update(float fDeltaTime);
	virtual void Destroy();

	/*
	 * This function will be removed later, when the rendering system will be more robust. But for now, we need it here.
	 * Function that will be called each frame to draw the entity
	*/
	void Draw(sf::RenderWindow& window) const;
private:
	std::string m_uuid;
	std::vector<IComponent*> m_components;
};
