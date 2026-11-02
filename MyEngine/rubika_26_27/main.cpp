#include <SFML/Graphics.hpp>
#include <Imgui/imgui.h>
#include <Imgui/imgui-SFML.h>

#include <Globals.h>
#include <Profiler.h>
#include <Logger.h>
#include <DebugMgr.h>

#include "RenderComponent.h"
#include "SceneMgr.h"

int main()
{
    gData.Init();

    /* TextureImport tests*/
    gData.TextureMgr->LoadTexture(
        "C:/Users/v.labrousse/Documents/GIT/rubika_26_27_cpp_public/MyEngine/Ressources/Antonio_Belpaese.png");
    gData.TextureMgr->LoadTexture(
        "C:/Users/v.labrousse/Documents/GIT/rubika_26_27_cpp_public/MyEngine/Ressources/IsaacSprite.png");
    gData.TextureMgr->LoadTexture(
        "C:/Users/v.labrousse/Documents/GIT/rubika_26_27_cpp_public/MyEngine/Ressources/unexistent.png");
    auto& texIsaac = gData.TextureMgr->GetTextureData("IsaacSprite");
    auto& texUnexistent = gData.TextureMgr->GetTextureData("blabla");
    /**/
    
    sf::RenderWindow window(sf::VideoMode({ 1080, 720 }), "MyEngine", sf::State::Windowed);

    ImGui::SFML::Init(window);
    
    sf::Clock clock;
    clock.restart();
    
    Scene* defaultScene = new Scene("Default Scene");
    Entity* eTheLash = new Entity("The Lash tm",
        sf::Vector2f(window.getSize().x/2, window.getSize().y/2));
    RenderComponent* renderComp1 = new RenderComponent(*eTheLash);
    eTheLash->AddComponent(renderComp1);
    defaultScene->AddUnique(eTheLash);
    gData.SceneMgr->AddUnique(defaultScene);
    
    
    /**
     * Main while loop
     */
    while (window.isOpen())
    {
        gData.IncrementFrameCount();

        PROFILER_EVENT_BEGIN(PROFILER_COLOR_BLACK, "Frame %llu", gData.FrameCount);

        int deltaTimeMicroS = clock.getElapsedTime().asMicroseconds();
        float fDeltaTimeMS = (float)deltaTimeMicroS / 1000.f;
        float fDeltaTimeS = fDeltaTimeMS / 1000.f;
        sf::Time imGuiTime = clock.restart();

        PROFILER_EVENT_BEGIN(PROFILER_COLOR_BLUE, "Event & Input");

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                
                window.close();
            }

#ifdef _USE_IMGUI
            ImGui::SFML::ProcessEvent(window, event.value());
#endif
        }

        PROFILER_EVENT_END();

        PROFILER_EVENT_BEGIN(PROFILER_COLOR_RED, "Update");
#ifdef _USE_IMGUI
        ImGui::SFML::Update(window, imGuiTime);
#endif

        // sample
        //ImGui::ShowDemoWindow();
        //Logger::DrawLogger();
        gData.DebugMgr->Draw();

        PROFILER_EVENT_END();

        PROFILER_EVENT_BEGIN(PROFILER_COLOR_GREEN, "Draw");
        window.clear();

#ifdef _USE_IMGUI
        ImGui::SFML::Render(window);
#endif
        window.display();

        PROFILER_EVENT_END();

        PROFILER_EVENT_END();
    }

    ImGui::SFML::Shutdown();

    // Shut
    gData.Shut();
    // Destroy
    gData.Destroy();

    return 0;
}
