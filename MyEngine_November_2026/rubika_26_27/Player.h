#pragma once
#include "Entity.h"

class Player : public Entity
{
public:
    Player(std::string friendlyName = "");
    virtual ~Player();

    virtual void Start() override;
};
