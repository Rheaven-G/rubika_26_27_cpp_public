#pragma once
#include "IComponent.h"

class RenderComponent : public IComponent
{
public:
	RenderComponent() = delete;
	RenderComponent(Entity& entity);
	~RenderComponent() override;
	
	void Start() override;
	void Update(float fDeltaTime) override;
	void Destroy() override;
};
