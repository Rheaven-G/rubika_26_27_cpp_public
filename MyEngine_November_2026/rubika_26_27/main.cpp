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
#include "Wall.h"


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

    const char* ptIsaac = "../../MyEngine/Ressources/IsaacSprite.png";
    gData.TextureMgr->LoadTexture(ptIsaac);
    const char* ptBasement = "../../MyEngine/Ressources/Basement.png";
    gData.TextureMgr->LoadTexture(ptBasement);

    Scene* scene = gData.SceneMgr->CreateScene("Main");
    {
        #define WALL_TOP_LEFT(posx, posy)       new Wall("Wall_TopLeft", "Corner_Top_Left", sf::Vector2f(posx, posy))
        #define WALL_TOP_RIGHT(posx, posy)      new Wall("Wall_TopRight", "Corner_Top_Right", sf::Vector2f(posx, posy))
        #define WALL_BOTTOM_LEFT(posx, posy)    new Wall("Wall_BottomLeft", "Corner_Bottom_Left", sf::Vector2f(posx, posy))
        #define WALL_BOTTOM_RIGHT(posx, posy)   new Wall("Wall_BottomRight", "Corner_Bottom_Right", sf::Vector2f(posx, posy))
        #define WALL_TOP(posx, posy)            new Wall("Wall_Top", "Wall_Classic_Up", sf::Vector2f(posx, posy))
        #define WALL_BOTTOM(posx, posy)         new Wall("Wall_Bottom", "Wall_Classic_Down", sf::Vector2f(posx, posy))
        #define WALL_LEFT(posx, posy)           new Wall("Wall_Left", "Wall_Classic_Left", sf::Vector2f(posx, posy))
        #define WALL_RIGHT(posx, posy)          new Wall("Wall_Right", "Wall_Classic_Right", sf::Vector2f(posx, posy))
        
        
        sf::Vector2i dims {3, 3};
        sf::Vector2i dTile {52, 52};
        
        std::vector<Wall*> wCorners;
        wCorners.push_back(WALL_TOP_LEFT(0, 0));
        wCorners.push_back(WALL_TOP_RIGHT(dims.x*dTile.x +dTile.x, 0));
        wCorners.push_back(WALL_BOTTOM_LEFT(0, dims.y*dTile.y +dTile.y));
        wCorners.push_back(WALL_BOTTOM_RIGHT(dims.x*dTile.x + dTile.x, dims.y*dTile.y + dTile.y));
        std::vector<Wall*> wTops;
        std::vector<Wall*> wBottoms;
        std::vector<Wall*> wLefts;
        std::vector<Wall*> wRights;
        for (int x {0}; x < dims.x; ++x)
            wTops.push_back(WALL_TOP(x*dTile.x +dTile.x, 0));
        for (int x {0}; x < dims.x; ++x)
            wBottoms.push_back(WALL_BOTTOM(x*dTile.x +dTile.x, dims.y*dTile.y +dTile.y));
        for (int y {0}; y < dims.y; ++y)
            wLefts.push_back(WALL_LEFT(0, y*dTile.y +dTile.y));
        for (int y {0}; y < dims.y; ++y)
            wRights.push_back(WALL_RIGHT(dims.x*dTile.x +dTile.x, y*dTile.y +dTile.y));
        
        for (Wall*& w : wCorners)
            scene->AddEntity(w);
        for (Wall*& w : wTops)
            scene->AddEntity(w);
        for (Wall*& w : wBottoms)
            scene->AddEntity(w);
        for (Wall*& w : wLefts)
            scene->AddEntity(w);
        for (Wall*& w : wRights)
            scene->AddEntity(w);
        
        Player* player = new Player("Player");
        scene->AddEntity(player);
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
