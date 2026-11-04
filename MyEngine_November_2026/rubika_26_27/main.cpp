#include <SFML/Graphics.hpp>
#include <Imgui/imgui.h>
#include <Imgui/imgui-SFML.h>

#include <Globals.h>
#include <TextureMgr.h>
#include <SceneMgr.h>
#include <Scene.h>
#include <Profiler.h>
#include <Logger.h>
#include <Entity.h>
#include <Player.h>
#include <TransformComponent.h>
#include <RenderComponent.h>
#include <SpriteDrawable.h>

#include "DynamicColliderComponent.h"
#include "InputSystem.h"
#include "PlayerComponent.h"


int main()
{
    gData.Init();

    sf::RenderWindow window(sf::VideoMode({ 1080, 720 }), "SFML works!", sf::State::Windowed);

    ImGui::SFML::Init(window);
    
    // ---- InputSystem
    InputSystem* is = gData.InputSystem;
    // Creation of input actions
    auto up = is->AddAction("MoveUP");
    auto down = is->AddAction("MoveDOWN");
    auto left = is->AddAction("MoveLEFT");
    auto right = is->AddAction("MoveRIGHT");
    // Binding of input actions with keyboard keys
    is->BindActionTo("MoveUP", {sf::Keyboard::Key::Up, -1});
    is->BindActionTo("MoveUP", {sf::Keyboard::Key::W, -1});
    is->BindActionTo("MoveDOWN", {sf::Keyboard::Key::Down, 1});
    is->BindActionTo("MoveDOWN", {sf::Keyboard::Key::R, 1});
    is->BindActionTo("MoveLEFT", {sf::Keyboard::Key::Left, -1});
    is->BindActionTo("MoveLEFT", {sf::Keyboard::Key::A, -1});
    is->BindActionTo("MoveRIGHT", {sf::Keyboard::Key::Right, 1});
    is->BindActionTo("MoveRIGHT", {sf::Keyboard::Key::S, 1});

    const char* texturePath = "../../MyEngine/Ressources/IsaacSprite.png";
    gData.TextureMgr->LoadTexture(texturePath);

    Scene* scene = gData.SceneMgr->CreateScene("Main");
    {
        Player* player = new Player("Player");
        scene->AddEntity(player);
        
        // Entity* entity = scene->CreateEntity("Isaac");
        //
        // TransformComponent* tc = entity->AddComponent<TransformComponent>();
        // {
        //     tc->SetWorldPosition(sf::Vector2f(100.f, 100.f));
        // }
        //
        // RenderComponent* rc = entity->AddComponent<RenderComponent>();
        // {
        //     SpriteDrawable* bodySprite = rc->AddNewDrawable<SpriteDrawable>("Body", sf::Vector2f(2, 0), 0, sf::Vector2f(1, 1));
        //     bodySprite->SetVisibility(true);
        //     bodySprite->SetTexture(texturePath);
        //     bodySprite->SetAnimation("Body_Vertical");
        //
        //     SpriteDrawable* headSprite = rc->AddNewDrawable<SpriteDrawable>("Head", sf::Vector2f(-2.5f, -20), 0, sf::Vector2f(1, 1));
        //     headSprite->SetVisibility(true);
        //     headSprite->SetTexture(texturePath);
        //     headSprite->SetAnimation("Head_Down");
        // }
        //
        // PlayerComponent* pc = entity->AddComponent<PlayerComponent>();
        // {}
        //
        // DynamicColliderComponent* dc = entity->AddComponent<DynamicColliderComponent>();
        // {}
    }
    gData.SceneMgr->SetActiveScene("Main");
    gData.SceneMgr->Start();

    sf::Clock clock;
    clock.restart();

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

        gData.Update(fDeltaTimeS);

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

        gData.SceneMgr->Draw(window);

#ifdef _USE_IMGUI
        ImGui::SFML::Render(window);
#endif
        window.display();

        PROFILER_EVENT_END();

        PROFILER_EVENT_END();
    }

    ImGui::SFML::Shutdown();

    gData.Shut();
    gData.Destroy();

    return 0;
}
