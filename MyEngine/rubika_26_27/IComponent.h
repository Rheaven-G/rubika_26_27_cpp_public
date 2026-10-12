#pragma once

class IComponent
{
public:
    virtual void Update(float fDeltaTime) = 0;
};
