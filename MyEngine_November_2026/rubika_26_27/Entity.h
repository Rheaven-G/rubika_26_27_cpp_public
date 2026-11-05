#pragma once

#include <string>
#include <vector>

class IComponent;

namespace sf
{
	class RenderWindow;
}

class Entity
{
public:
	enum TAG
	{
		NONE    = 0,
		STATIC  = 1 << 0, // is not static, it's dynamic
		NEUTRAL = 1 << 1,
		ENEMY   = 1 << 2,
		PLAYER  = 1 << 3
	};

	Entity(std::string friendlyName = "");
	virtual ~Entity();

	virtual void Start();
	virtual void Update(float fDeltaTime);
	virtual void Destroy();

	template <typename C>
	C* AddComponent();

	template <typename C>
	C* GetComponent() const;

	/*
	 * This function will be removed later, when the rendering system will be more robust. But for now, we need it here.
	 * Function that will be called each frame to draw the entity
	*/
	void Draw(sf::RenderWindow& window) const;
	void DrawDebug();

	const std::string& GetName() const { return FriendlyName; }
	
	void AddTag(const TAG& tag)
	{
		Tags |= tag;
	}
	void RemoveTag(const TAG& tag)
	{
		Tags &= ~tag;
	}
	/**
	 * Checks if Entity.Tags contains one of tags
	 * @param tags TAG&
	 * @return 
	 */
	bool HasTag(const TAG& tags) const
	{
		return (Tags & tags) != 0;
	}
	/**
	 * Compares if Entity.Tags has exact tags
	 * @param tags TAG&
	 * @return 
	 */
	short CompareTagsWith(const TAG& tags) const
	{
		return (Tags & tags) == tags;
	}

protected:
	std::string FriendlyName;
	std::vector<IComponent*> Components;
	short Tags = TAG::NONE; // 16 flags
};

#include <Entity.hxx>
