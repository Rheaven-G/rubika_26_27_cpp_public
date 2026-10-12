#include "Scene.h"

#include <format>
#include <shobjidl_core.h>

#include "utility.h"

Scene::Scene(const std::string& name)
{
    //m_uuid = uuid::generated_uuid_v4();
    friendlyName = name;
}
Scene::~Scene()
{
    Destroy();
}

std::string Scene::getUUID() const { return m_uuid;}

void Scene::Start() {}
void Scene::Update(float fDeltaTime)
{
    for (Entity*& p_entity : m_entities)
    {
        p_entity->Update(fDeltaTime);
    }
}
void Scene::Destroy()
{
    for (Entity*& p_entity : m_entities) 
        p_entity->Destroy();
    m_entities.clear();
}

Entity* Scene::FindByName(const std::string& name)
{
    for (Entity*& p_entity : m_entities)
        if (p_entity->friendlyName == name) return p_entity;
    
    return nullptr;
}
std::vector<Entity*> Scene::GetAllEntities() { return m_entities; }
void Scene::AddUnique(Entity* p_entity)
{
    int n = 0;
    std::string name = p_entity->friendlyName;
    std::vector<std::string> names;
    for (Entity*& p_e : m_entities) 
        names.push_back(p_e->friendlyName);
    while (!IsNameUnique(name, names))
    {
        n++;
        name = std::format("{} {}", p_entity->friendlyName, n);
    }
    p_entity->friendlyName = name;
    
    m_entities.push_back(p_entity);
}
void Scene::Remove(const Entity* p_entity)
{
    for (size_t i = 0; i < m_entities.size(); ++i)
    {
        if (m_entities[i] == p_entity) 
            m_entities.erase(m_entities.begin() + i);
    }
}

void Scene::Draw(sf::RenderWindow& window) const
{
    for (Entity* p_entity : m_entities)
    {
        p_entity->Draw(window);
    }
}

// static non-member
bool IsNameUnique(const std::string& name, const std::vector<std::string>& names)
{
    for (const std::string& str : names)
    {
        if (str == name) 
            return false;
    }
    return true;
}