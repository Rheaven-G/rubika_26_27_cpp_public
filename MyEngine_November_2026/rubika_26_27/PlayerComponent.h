#pragma once
#include "IComponent.h"
#include "TransformComponent.h"


class PlayerComponent : public IComponent
{
public:
    // IComponent
    PlayerComponent(Entity& entity);
    ~PlayerComponent() override;
    
    virtual void Start() override;
    virtual void Update(float fDeltaTime) override;
    virtual void Destroy() override;
    
    const char* GetName() const override { return "PlayerComponent";}
    void DrawDebug() override;
    
private:
    float MovementSpeed{ .03f };
    
    void OnMoveX(float val);
    void OnMoveY(float val);
    void OnShootX(float val);
    void OnShootY(float val);
    
    
    TransformComponent* Transform;
};
