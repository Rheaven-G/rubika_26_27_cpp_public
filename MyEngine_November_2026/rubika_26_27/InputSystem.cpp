#include "InputSystem.h"

#include <format>
#include <SFML/System/String.hpp>

#include "Globals.h"
#include "Logger.h"
#include "Profiler.h"
#include "ImGui/imgui.h"

void InputSystem::Init()
{
    gData.DebugMgr->RegisterDebugableWindow("InputSystem", this);
}
void InputSystem::Start() {}
void InputSystem::Update(float deltaTime)
{
    for (auto pair : ActionBindings)
    {
        InputAction& action = *pair.first;
        std::vector<Binding>& actionBindings = pair.second;
        
        for (auto binding : actionBindings)
        {
            if (sf::Keyboard::isKeyPressed(binding.Key))
            {
                action.SetValue(binding.ValueInfluence);
            }
            else
            {
                action.SetValue(0.f);
            }
        }
        
        // Then we check if we held...
        action.CheckEvents();
    }
}
void InputSystem::Shut()
{
    
    gData.DebugMgr->UnregisterDebugableWindow("InputSystem");
}


InputAction* InputSystem::AddAction(const std::string& actionName)
{
    Actions.insert({actionName, {actionName}});
    return FindAction(actionName);
}
/**
 * Not the fastest, but I assume we are not going to use it a lot.
 * Comfort > Cost for the current decision.
 * I may regret it...
 * 
 * @param actionName std::string
 * @return InputAction
 */
InputAction* InputSystem::FindAction(const std::string& actionName)
{
    auto it = Actions.find(actionName);
    return it != Actions.end() 
        ? &it->second 
        : nullptr;
}
std::string InputSystem::FindActionName(const InputAction* action)
{
    for (auto& act : Actions)
    {
        if (&act.second == action)
            return act.first;
    }
}
std::vector<Binding>* InputSystem::FindBindingsOf(const std::string& actionName)
{
    InputAction* action = FindAction(actionName);
    if (action != nullptr)
    {
        auto it = ActionBindings.find(action);
        if (it != ActionBindings.end())
        {
            return &it->second;
        }
    }
    return nullptr;
}
void InputSystem::BindActionTo(const std::string& actionName, Binding binding)
{
    auto action = FindAction(actionName);
    auto find = FindBindingsOf(actionName);
    if (action != nullptr)
    {
        if (find != nullptr)
        {
            find->push_back(binding);
            return;
        } 
        else
        {
            ActionBindings.insert({action, {binding}});
            return;
        }
    }
    Logger::Error(std::format(
        "Couldn't bind key to action because '{}' doesn't exists.", actionName));
}
void InputSystem::UnbindActionFrom(const std::string& actionName, Binding binding)
{
    auto action = FindAction(actionName);
    auto find = FindBindingsOf(actionName);
    if (action != nullptr)
    {
        if (find != nullptr)
        {
            for (int i {0}; i < find->size(); i++)
            {
                if (find->at(i).Key == binding.Key)
                    find->erase(find->begin() + i);
            }
        } 
    }
    Logger::Error(std::format(
        "Couldn't unbind key from '{}' because action doesn't exists.", actionName));
}

void InputSystem::DrawDebug()
{
    for (auto nameActionPair : Actions)
    {
        std::string actionName = nameActionPair.first;
        InputAction& action = nameActionPair.second;
        
        ImGui::PushID(actionName.c_str());
        
        
        ImGui::TextUnformatted(std::format(
              "{}", 
              action.GetValue()).c_str());
        ImGui::SameLine();
          ImGui::SeparatorText(actionName.c_str());
       
        if (ImGui::TreeNode("Bindings"))
        {
            std::vector<Binding>& bindings = *FindBindingsOf(actionName);
            for (size_t i {0}; i < bindings.size(); i++)
            {
                std::string rp = 
                    sf::Keyboard::getDescription( sf::Keyboard::delocalize(bindings[i].Key));
                ImGui::TextUnformatted(rp.c_str());
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Callbacks"))
        {
            for (auto callback : action.GetCallbacks())
            {
                ImGui::TextUnformatted(callback.DebugName.c_str());
                //ImGui::TextUnformatted(typeid(callback.Callback).name());
            }
            ImGui::TreePop();
        }
        
        ImGui::PopID();
    }
}