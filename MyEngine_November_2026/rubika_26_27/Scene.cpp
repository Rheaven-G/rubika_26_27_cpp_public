#include "Scene.h"

#include <Entity.h>

#include <Imgui/imgui.h>

#include "DynamicColliderComponent.h"
#include "RenderComponent.h"

Scene::Scene(const std::string& name) : Name(name)
{
	Hash = std::hash<std::string>{}(Name);
}

Scene::~Scene()
{}

void Scene::Start()
{
	for (Entity* e : Entities)
	{
		e->Start();
	}
}

void Scene::Update(float fDeltaTime)
{
	for (Entity* e : Entities)
	{
		e->Update(fDeltaTime);
	}
	
	FindOverlaps(fDeltaTime);
	
	// Black Magic~~~
	std::function<std::vector<RenderComponent*>()> rcs = [this]()
	{
		std::vector<RenderComponent*> rcs;
		for (Entity* e : Entities)
		{
			RenderComponent* rc = e->GetComponent<RenderComponent>();
			if (rc != nullptr)
				rcs.push_back(rc);
		}
		return rcs;
	};
	for (int i {0}; i < rcs().size(); ++i) 
	{
		rcs()[i]->Update(fDeltaTime);
	}
}

void Scene::Destroy()
{
	for (Entity* e : Entities)
	{
		e->Destroy();
	}
}

Entity* Scene::CreateEntity(const std::string& entityName)
{
	Entity* e = new Entity(entityName);
	Entities.push_back(e);
	return e;
}

void Scene::AddEntity(Entity* entity)
{
	Entities.push_back(entity);
}

void Scene::RemoveEntity(Entity* entity)
{
	for (auto it = Entities.begin(); it != Entities.end(); ++it)
	{
		if (entity == *it)
		{
			entity->Destroy();
			it = Entities.erase(it);
		}
	}
}

void Scene::Draw(sf::RenderWindow& window) const
{
	for (Entity* e : Entities)
	{
		e->Draw(window);
	}
}

void Scene::DrawDebug()
{
	for (Entity* e : Entities)
	{
		ImGui::PushID(e);

		const char* preview = e->GetName().size() ? e->GetName().c_str() : "Entity";
		if (ImGui::TreeNode(preview))
		{
			e->DrawDebug();
			ImGui::TreePop();
		}

		ImGui::PopID();
	}
}

void Scene::FindOverlaps(float fDeltaTime)
{
	for (auto* e1 : Entities)
	{
		auto* dcc1 = e1->GetComponent<DynamicColliderComponent>();
		if (dcc1 == nullptr) // Does e1 have a Collider
			continue;
		if ((dcc1->Flags & DynamicColliderComponent::FLAGS::ENABLED) == 0) // Is Collider Disabled ?
			continue;
		
		for (auto* e2 : Entities)
		{
			auto* dcc2 = e2->GetComponent<DynamicColliderComponent>();
			if (dcc2 == nullptr || dcc1 == dcc2) // Does e2 have a Collider and is it == e1 ?
				continue;
			if ((dcc2->Flags & DynamicColliderComponent::FLAGS::ENABLED) == 0) // Is Collider Disabled ?
				continue;
			
			// Else there is overlap
			auto intersection = dcc1->GetHitbox().findIntersection(dcc2->GetHitbox());
			if (intersection.has_value())
			{
				// If both are not TRIGGER, there is collision
				if ((dcc1->Flags & DynamicColliderComponent::FLAGS::TRIGGER) == 0 
					&& (dcc2->Flags & DynamicColliderComponent::FLAGS::TRIGGER) == 0)
				{
					sf::Vector2f dir1 {
						dcc1->GetHitbox().getCenter().x < intersection.value().getCenter().x ? -1.f : 
							dcc1->GetHitbox().getCenter().x > intersection.value().getCenter().x ? 1.f : 0.f
						* fDeltaTime,
						dcc1->GetHitbox().getCenter().y < intersection.value().getCenter().y ? -1.f : 
							dcc1->GetHitbox().getCenter().y > intersection.value().getCenter().y ? 1.f : 0.f
						* fDeltaTime
					};
					sf::Vector2f dir2 {
						dcc2->GetHitbox().getCenter().x < intersection.value().getCenter().x ? -1.f : 
							dcc2->GetHitbox().getCenter().x > intersection.value().getCenter().x ? 1.f : 0.f
						* fDeltaTime,
						dcc2->GetHitbox().getCenter().y < intersection.value().getCenter().y ? -1.f : 
							dcc2->GetHitbox().getCenter().y > intersection.value().getCenter().y ? 1.f : 0.f
						* fDeltaTime
					};
					// If they are not static, we move them.
					if (!e1->HasTag(Entity::TAG::STATIC))
						dcc1->UpdateTransform(dir1);
					if (!e2->HasTag(Entity::TAG::STATIC))
						dcc2->UpdateTransform(dir2);
				}
				
				if ((dcc1->Flags & DynamicColliderComponent::FLAGS::TRIGGER) != 0)
					dcc1->OnTrigger(dcc2);
				else
					dcc1->OnCollision(dcc2);
				
				if ((dcc2->Flags & DynamicColliderComponent::FLAGS::TRIGGER) != 0)
					dcc2->OnTrigger(dcc1);
				else
					dcc2->OnCollision(dcc1);
			}
		}
	}
}