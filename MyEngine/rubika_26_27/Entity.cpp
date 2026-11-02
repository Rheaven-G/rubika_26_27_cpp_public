#include "Entity.h"

#include <format>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Logger.h"
#include "utility.h"
#include "TransformComponent.h"

Entity::Entity(std::string name, sf::Vector2f position) : transform(TransformComponent(*this))
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

// ---- Getters
std::string Entity::GetUUID() { return m_uuid; }
// ---- Setters
// TODO: not really a todo but I would like it to work...
// template <typename C>
// C* Entity::AddComponent()
// {
//     /* was adding a IComponent* before
//     for (const auto& comp : m_components)
//     {
//         if (comp == component)
//         {
//             Logger::Error(std::format(
//                 "Entity::AddComponent -- The very IComponent '{}'({}) already exists in Entity {}.", 
//                 typeid(*comp).name(),
//                 //comp,
//                 //friendlyName));
//                 "B", "C"));
//             return nullptr;
//         }
//     }*/
//     C* component = new C(*this);
//     m_components.push_back(component);
//     return component;
// }
// template<>
IComponent* Entity::AddComponent(IComponent* component)
{
    m_components.push_back(component);
    return component;
}

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
    // for (size_t i = m_components.size(); i > 0; --i)
    // {
    //     delete m_components[i -1];
    //     m_components[i -1] = nullptr;
    // }
    for (auto component : m_components)
    {
        delete component;
        component = nullptr;
    }
    m_components.clear();
}

void Entity::Draw(sf::RenderWindow& window) const
{
}