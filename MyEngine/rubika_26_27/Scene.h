#pragma once

#include <string>
#include "Entity.h"

namespace sf
{
	class RenderWindow;
}

class Scene
{
	std::string m_uuid;
	std::vector<Entity*> m_entities;
public:
	std::string friendlyName;

	Scene(const std::string& name);
	~Scene();
	
	std::string getUUID() const;

	virtual void Start();
	virtual void Update(float fDeltaTime);
	virtual void Destroy();

	Entity* FindByName(const std::string& name);
	std::vector<Entity*> GetAllEntities();
	void AddUnique(Entity* entity);
	void Remove(const Entity* entity);
	
	
	/*
	 * This function will be removed later, when the rendering system will be more robust. But for now, we need it here.
	 * Function that will be called each frame to draw the entity
	*/
	void Draw(sf::RenderWindow& window) const;
};
bool IsNameUnique(const std::string& name, const std::vector<std::string>& names);