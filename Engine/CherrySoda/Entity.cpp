#include <CherrySoda/Entity.h>

#include <CherrySoda/Scene.h>
#include <CherrySoda/Colliders/Collide.h>
#include <CherrySoda/Colliders/Collider.h>
#include <CherrySoda/Components/Component.h>
#include <CherrySoda/InternalUtilities/ComponentList.h>
#include <CherrySoda/InternalUtilities/EntityList.h>
#include <CherrySoda/InternalUtilities/TagLists.h>
#include <CherrySoda/Util/Profile.h>
#include <CherrySoda/Util/Math.h>
#include <CherrySoda/Util/STL.h>

using cherrysoda::Entity;

using cherrysoda::Camera;
using cherrysoda::Collide;
using cherrysoda::Collider;
using cherrysoda::Component;
using cherrysoda::ComponentList;
using cherrysoda::Math;
using cherrysoda::Scene;
using cherrysoda::STL;

Entity::Entity(const Math::Vec3& position)
{
	m_position = position;
	m_components = new ComponentList(this);
}

void Entity::SceneBegin(Scene* scene)
{
}

void Entity::SceneEnd(Scene* scene)
{
	for (auto component : *m_components) {
		component->SceneEnd(scene);
	}
}

void Entity::Awake(Scene* scene)
{
	for (auto component : *m_components) {
		component->EntityAwake();
	}
}

void Entity::Update()
{
	CHERRYSODA_PROFILE_FUNCTION();

	m_components->Update();
}

void Entity::Render()
{
	CHERRYSODA_PROFILE_FUNCTION();

	m_components->Render();
}

void Entity::Tag(int tag)
{
	if (m_tag == tag)
	{
		return;
	}
	if (m_scene != nullptr)
	{
		for (int i = 0; i < BitTag::TotalTags(); ++i)
		{
			int num = 1 << i;
			bool flag = (tag & num) != 0;
			if ((m_tag & num) != 0 != flag)
			{
				if (flag)
				{
					STL::Add(m_scene->Tags()->operator[](i), this);
				}
				else
				{
					STL::Remove(m_scene->Tags()->operator[](i), this);
				}
			}
		}
	}
	m_tag = tag;	
}

const STL::List<Entity*> Entity::CollideAll(const BitTag& tag) const
{
#ifndef NDEBUG
	if (m_scene == nullptr) {
		CHERRYSODA_ASSERT(false, "Can't collide check an Entity against a tag list when it is not a member of a Scene\n");
	}
#endif
	return Collide::All(this, (*m_scene)[tag]);
}

bool Entity::CollideCheck(const BitTag& tag) const
{
#ifndef NDEBUG
	if (m_scene == nullptr) {
		CHERRYSODA_ASSERT(false, "Can't collide check an Entity against a tag list when it is not a member of a Scene\n");
	}
#endif
	return Collide::Check(this, (*m_scene)[tag]);
}

int Entity::CollideCount(const BitTag& tag) const
{
#ifndef NDEBUG
	if (m_scene == nullptr) {
		CHERRYSODA_ASSERT(false, "Can't collide check an Entity against a tag list when it is not a member of a Scene\n");
	}
#endif
	return Collide::Count(this, (*m_scene)[tag]);
}

Entity* Entity::CollideFirst(const BitTag& tag) const
{
#ifndef NDEBUG
	if (m_scene == nullptr) {
		CHERRYSODA_ASSERT(false, "Can't collide check an Entity against a tag list when it is not a member of a Scene\n");
	}
#endif
	return Collide::First(this, (*m_scene)[tag]);
}

void Entity::Add(Component* component)
{
	m_components->Add(component);
}

void Entity::Remove(Component* component)
{
	m_components->Remove(component);
}

void Entity::Add(ComponentList::IterableComponents& components)
{
	m_components->Add(components);
}

void Entity::Remove(ComponentList::IterableComponents& components)
{
	m_components->Remove(components);
}

void Entity::Depth(int depth)
{
	if (m_depth != depth) {
		m_depth = depth;
		if (m_scene != nullptr) {
			m_scene->_SetActualDepth(this);
		}
	}
}

void Entity::DebugRender(Camera* camera)
{
	// TODO: collider debugrender
	m_components->DebugRender(camera);
}

void Entity::HandleGraphicsReset()
{
	m_components->HandleGraphicsReset();
}

void Entity::HandleGraphicsCreate()
{
	m_components->HandleGraphicsCreate();
}

void Entity::RemoveSelf()
{
	if (m_scene != nullptr) {
		m_scene->Entities()->Remove(this);
	}
}

void Entity::Added(Scene* scene)
{
	m_scene = scene;
	for (auto component : *m_components) {
		component->EntityAdded(scene);
	}
	m_scene->_SetActualDepth(this);
}

void Entity::Removed(Scene* scene)
{
	for (auto component : *m_components) {
		component->EntityRemoved(scene);
	}
	m_scene = nullptr;
}

void Entity::SetCollider(Collider* collider)
{
	if (m_collider == collider) {
		return;
	}
#ifndef NDEBUG
	if (collider->GetEntity() != nullptr) {
		CHERRYSODA_ASSERT(false, "Setting an Entity's Collider to a Collider already in use by another object\n")
	}
#endif
	if (m_collider != nullptr) {
		m_collider->Removed();
	}
	m_collider = collider;	
	if (m_collider) {
		m_collider->Added(this);
	}
}
