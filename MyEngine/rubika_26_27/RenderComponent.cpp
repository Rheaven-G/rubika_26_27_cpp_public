#include "RenderComponent.h"

RenderComponent::RenderComponent(Entity& entity) : IComponent(entity) {}
RenderComponent::~RenderComponent() {}

void RenderComponent::Start() {}
void RenderComponent::Update(float fDeltaTime) {}
void RenderComponent::Destroy() {}