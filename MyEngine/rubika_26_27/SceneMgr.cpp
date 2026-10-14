#include "SceneMgr.h"
#include <format>
#include "Globals.h"
static void TreeNodeExRecursive(Entity*& p_entity, ImGuiTreeNodeFlags flags);


SceneMgr::~SceneMgr()
{
    Destroy();
}

void SceneMgr::Start() {}
void SceneMgr::Update(float fDeltaTime) {}
void SceneMgr::Destroy()
{
    for (Scene*& p_scene : m_scenes)
    {
        p_scene->Destroy();
    }
    if (m_activeScene != nullptr) m_activeScene->Destroy();
    m_scenes.clear();
}

void SceneMgr::SetActive(size_t index)
{
    if (m_scenes.size() <= index)
    {
        Logger::Error("SceneMgr::SetActive -> index scene doesn't exist. [INDEX_OUT_OF_RANGE]");
        return;
    }
    if (m_scenes[index] != nullptr)
    {
        m_activeScene = m_scenes[index];
    }
    else Logger::Error("SceneMgr::SetActive -> scene pointer is nullptr.");
}
void SceneMgr::SetActive(Scene* p_scene)
{
    for (Scene*& p_s : m_scenes)
    {
        if (p_s == p_scene) {
            m_activeScene = p_s;
            return;
        }
    }
    Logger::Error("SceneMgr::SetActive -> scene doesn't exist in sceneList. You should add it first with SceneMgr::AddUnique().");
} 
void SceneMgr::SetActive(std::string sceneName)
{
    bool foundit = false;
    Scene* p_foundScene = nullptr;
    for (Scene*& p_scene : m_scenes)
    {
        if (p_scene->friendlyName == sceneName)
        {
            if (foundit)
            {
                Logger::Error("SceneMgr::SetActive -> There are multiple scenes with this name. Prefer using a pointer or index");
                return;
            }
            foundit = true;
            p_foundScene = p_scene;
        }
    }
    if (foundit) m_activeScene = p_foundScene;
    else Logger::Error("SceneMgr::SetActive -> There is no Scene with this name.");
}

Scene* SceneMgr::GetActive() const
{
    return m_activeScene;
}
size_t SceneMgr::GetActiveIndex()
{
    for (size_t i = 0; i < m_scenes.size(); i++)
    {
        if (m_scenes[i] == m_activeScene) 
            return i;
    }
    return 0;
}

void SceneMgr::AddUnique(Scene* p_scene)
{
    int n = 0;
    std::string name = p_scene->friendlyName;
    std::vector<std::string> names;
    for (Scene*& p_s : m_scenes) 
        names.push_back(p_s->friendlyName);
    while (!IsNameUnique(name, names))
    {
        n++;
        name = std::format("{} {}", p_scene->friendlyName, n);
    }
    p_scene->friendlyName = name;
    
    m_scenes.push_back(p_scene);
    if (m_activeScene == nullptr) SetActive(m_scenes.back());
}
void SceneMgr::Remove(const Scene* p_scene)
{
    for (size_t i = 0; i < m_scenes.size(); i++)
    {
        if (m_scenes[i] == p_scene)
        {
            m_scenes.erase(m_scenes.begin() + i);
        } 
        if (m_activeScene == p_scene)
        {
            m_activeScene->Destroy();
            if (m_scenes.size() == 0)
                AddUnique(new Scene("Empty Scene"));
            SetActive(m_scenes.back());
        }
    }
}

// IDebugable
void SceneMgr::Init()
{
    gData.DebugMgr->RegisterDebugableWindow("SceneMgr", this);
}
void SceneMgr::Shut()
{
    gData.DebugMgr->UnregisterDebugableWindow("SceneMgr");
}
void SceneMgr::DrawDebug()
{
#ifdef _USE_IMGUI
    // Head
    size_t current_item = GetActiveIndex();
    std::vector<const char*> items;
    items.reserve(m_scenes.size());
    for (size_t i = 0; i < m_scenes.size(); i++)
    {
        items.push_back(m_scenes[i]->friendlyName.c_str());
    }
    if (ImGui::BeginCombo("Scenes##SceneMgr", items[current_item]))
    {
        for (size_t i = 0; i < m_scenes.size(); i++)
        {
            const bool is_selected = (current_item == i);
            if (ImGui::Selectable(items[i], is_selected))
                SetActive(i);
            
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine(); if (ImGui::Button(" - ##SceneMgr"))
    {
        Remove(m_activeScene);
    } 
    ImGui::SameLine(); if (ImGui::Button(" + ##SceneMgr"))
    {
        AddUnique(new Scene("Empty Scene"));
    }
    
    if (ImGui::Button(" + ##Scene"))
    {
        m_activeScene->AddUnique(new Entity("Entity", sf::Vector2f(0.0f, 0.0f )));
    }
    ImGui::SameLine(); ImGui::SeparatorText("Hierarchy");
    // TreeView
    static ImGuiTreeNodeFlags base_flags = 
        ImGuiTreeNodeFlags_DrawLinesFull |
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick|
        ImGuiTreeNodeFlags_SpanAvailWidth;
    ImGui::SetNextItemOpen(true);
    if (ImGui::TreeNode(m_activeScene->friendlyName.c_str()))
    {
        for (Entity*& p_entity : m_activeScene->GetAllEntities())
        {
            TreeNodeExRecursive(p_entity, base_flags);
        }
        ImGui::TreePop(); // Mandatory in each if of TreeNode / TreeNodeEx
    }
#endif
}

// static non-member
static void TreeNodeExRecursive(Entity*& p_entity, ImGuiTreeNodeFlags flags)
{
#ifdef _USE_IMGUI
    if (p_entity != nullptr)
    {
        if (p_entity->children.empty())
        {
            ImGui::BulletText("%s", p_entity->friendlyName.c_str());
            return;
        }
        if (ImGui::TreeNodeEx(p_entity->friendlyName.c_str(), flags))
        {
            if (!p_entity->children.empty())
            {
                for (Entity*& p_child : p_entity->children)
                {
                    TreeNodeExRecursive(p_child, flags);
                }
            }
            ImGui::TreePop();
        }
    }
    
    return;
#endif
}
