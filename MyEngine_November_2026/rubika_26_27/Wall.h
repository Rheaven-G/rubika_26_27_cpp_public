#pragma once
#include <SFML/System/Vector2.hpp>

#include "Entity.h"

class Wall : public Entity
{
public:
    Wall(std::string friendlyName = "", 
        std::string tileName = "",
        sf::Vector2f pos = sf::Vector2f(0, 0));
    virtual ~Wall();

    virtual void Start() override;
    
    void SetPosition(sf::Vector2f pos);
    
private:
    std::string tileName;
};
