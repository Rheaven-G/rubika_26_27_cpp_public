#pragma once
#include <SFML/Window/Keyboard.hpp>

#include "DebugMgr.h"
#include "InputAction.h"

// Event tho it's an `.inputsystem`, it is an xml file
const std::string DEFAULT_MAPPING_PATH{ "../../../MyEngine/Ressources/InputMappingAsset.inputsystem" };

struct Binding
{
public:
    sf::Keyboard::Key Key;
    float ValueInfluence;
};

class InputSystem : public IDebugable
{
public:
    
    void Init();
    void Start();
    void Update(float fDeltaTime);
    void Shut();
    
    virtual void DrawDebug() override;

    InputAction* AddAction(const std::string& actionName);
    InputAction* FindAction(const std::string& actionName);
    std::string  FindActionName(const InputAction* action);
    std::vector<Binding>* FindBindingsOf(const std::string& actionName);
    void BindActionTo(const std::string& actionName, Binding binding);
    void UnbindActionFrom(const std::string& actionName, Binding binding);
    
private:
    // bool DoesMappingFileExists();
    // bool IsMappingFileConform();
    // bool TryReadMappingFile();
    
    std::string MappingFile { DEFAULT_MAPPING_PATH };
    
    std::unordered_map<std::string, InputAction> Actions;
    std::unordered_map<InputAction*, std::vector<Binding>> ActionBindings;
};

