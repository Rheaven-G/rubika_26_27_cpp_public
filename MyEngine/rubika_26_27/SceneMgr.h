#pragma once
#include "Scene.h"
#include "Logger.h"
#include "DebugMgr.h"
#include "ImGui/imgui.h"

class SceneMgr : public IDebugable
{
private:
    std::vector<Scene*> m_scenes;
    Scene* m_activeScene;
    
    bool m_imgui_active = true;
    
public:
    SceneMgr() : m_activeScene(nullptr) {}
    ~SceneMgr();
    
    void Start();
    void Update(float fDeltaTime);
    void Destroy();
    
    void SetActive(size_t index); // comparing with std::vector.size()
    void SetActive(Scene* scene);
    void SetActive(std::string sceneName);
    
    Scene* GetActive() const;
    size_t GetActiveIndex();
    
    void AddUnique(Scene* p_scene);
    void Remove(const Scene* p_scene);
    
    // IDebugable
    void Init();
    void Shut();
    virtual void DrawDebug() override;
};
