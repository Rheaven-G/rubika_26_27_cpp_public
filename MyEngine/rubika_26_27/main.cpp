#include <SFML/Graphics.hpp>
#include <Imgui/imgui.h>
#include <Imgui/imgui-SFML.h>

#include <Globals.h>
#include <Profiler.h>
#include <Logger.h>
#include <DebugMgr.h>

#include "SceneMgr.h"

int main()
{
    gData.Init();

    sf::RenderWindow window(sf::VideoMode({ 1080, 720 }), "MyEngine", sf::State::Windowed);

    ImGui::SFML::Init(window);
    
    /* Test logs
    Logger::Info("This is an info message. That is useful");
    Logger::Warning("This is a warning message. Should I worry?");
    Logger::Error("This is an error message. Oh sh*t!!");
    */

    sf::Clock clock;
    clock.restart();
    
    /**
     * Scene init + entities
     */
    /* Previous testing, Threshold 1
    SceneMgr sceneMgr;
    sceneMgr.Init();
    Scene* s1 = new Scene("DefaultScene1");
    Scene* s2 = new Scene("DefaultScene2");
    sceneMgr.AddUnique(s1);
    sceneMgr.AddUnique(s2);
    sceneMgr.SetActive(s1);
    sceneMgr.SetActive("DefaultScene"); // Error testing
    sceneMgr.SetActive("IncredibleScene"); // Error testing
    Entity* e1 = new Entity("myEntity 1", sf::Vector2f(0.0f, 0.0f));
    Entity* e2 = new Entity("myEntity 2", sf::Vector2f(0.0f, 0.0f));
    Entity* e3 = new Entity("anotherOne", sf::Vector2f(0.0f, 0.0f));
    e2->children.push_back(e3);
    Entity* e4 = new Entity("anotherOne", sf::Vector2f(0.0f, 0.0f));
    e2->children.push_back(e4);
    Entity* e5 = new Entity("I lied", sf::Vector2f(0.0f, 0.0f));
    e3->children.push_back(e5);
    sceneMgr.GetActive()->AddUnique(e1);
    sceneMgr.GetActive()->AddUnique(e2);
    sceneMgr.GetActive()->Remove(e1);
    sceneMgr.GetActive()->AddUnique(e1);
    */
    Scene* defaultScene = new Scene("Default Scene");
    defaultScene->AddUnique(new Entity("The Lash tm", sf::Vector2f(0.0f, 0.0f)));
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
