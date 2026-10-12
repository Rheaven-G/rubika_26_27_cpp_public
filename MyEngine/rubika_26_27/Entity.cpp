#include "Entity.h"

#include <format>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Logger.h"
#include "utility.h"
#include "TransformComponent.h"

Entity::Entity(std::string name, sf::Vector2f position)
{
    //m_uuid = uuid::generated_uuid_v4();
    friendlyName = name;
    transform.SetWorldPosition(position);
}
Entity::~Entity()
{
    Destroy();
}
// OPERATORS
bool Entity::operator==(Entity const& other) const
{
    return m_uuid == other.m_uuid;
}

std::string Entity::getUUID() { return m_uuid; }
void Entity::Start() {}
void Entity::Update(float fDeltaTime)
{
    for (size_t i = 0; i < m_components.size(); ++i)
    {
        m_components[i]->Update(fDeltaTime);
    }
}
void Entity::Destroy()
{
    for (size_t i = m_components.size(); i > 0; --i)
    {
        delete m_components[i -1];
    }
    m_components.clear();
}

void Entity::Draw(sf::RenderWindow& window) const
{
}