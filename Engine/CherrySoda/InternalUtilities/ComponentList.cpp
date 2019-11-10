#include <CherrySoda/InternalUtilities/ComponentList.h>

#include <CherrySoda/Components/Component.h>
#include <CherrySoda/Util/Log.h>
#include <CherrySoda/Util/STL.h>

using cherrysoda::ComponentList;

using cherrysoda::Camera;
using cherrysoda::Component;
using cherrysoda::Entity;
using cherrysoda::STL;

ComponentList::ComponentList(Entity* entity)
{
	m_entity = entity;
	m_lockMode = LockModes::Open;
}

void ComponentList::LockMode(LockModes lockMode)
{
	m_lockMode = lockMode;

	if (STL::Count(m_toAdd) > 0) {
		for (auto component : m_toAdd) {
			if (!STL::Contains(m_current, component)) {
				STL::Add(m_current, component);
				STL::Add(m_components, component);
				component->Added(GetEntity());
			}
		}

		m_adding.clear();
		m_toAdd.clear();
	}

	if (STL::Count(m_toRemove) > 0) {
		for (auto component : m_toRemove) {
			if (STL::Contains(m_current, component)) {
				STL::Remove(m_current, component);
				STL::Remove(m_components, component);
				component->Removed(GetEntity());
			}
		}

		m_removing.clear();
		m_toRemove.clear();
	}
}

void ComponentList::Add(Component* component)
{
	switch (m_lockMode) {
	case LockModes::Open:
		if (!STL::Contains(m_current, component)) {
			STL::Add(m_current, component);
			STL::Add(m_components, component);
			component->Added(GetEntity());
		}
		break;

	case LockModes::Locked:
		if (!STL::Contains(m_current, component) && !STL::Contains(m_adding, component)) {
			STL::Add(m_adding, component);
			STL::Add(m_toAdd, component);
		}
		break;

	case LockModes::Error:
		CHERRYSODA_LOG("Cannot add or remove Entities at this time!");
	}
}

void ComponentList::Remove(Component* component)
{
	switch (m_lockMode) {
	case LockModes::Open:
		if (STL::Contains(m_current, component)) {
			STL::Remove(m_current, component);
			STL::Remove(m_components, component);
			component->Removed(GetEntity());
		}
		break;

	case LockModes::Locked:
		if (STL::Contains(m_current, component) && !STL::Contains(m_removing, component)) {
			STL::Add(m_removing, component);
			STL::Add(m_toRemove, component);
		}
		break;

	case LockModes::Error:
		CHERRYSODA_LOG("Cannot add or remove Entities at this time!");
	}
}

void ComponentList::Add(ComponentList::IterableComponents& components)
{
	for (auto component : components) {
		Add(component);
	}
}

void ComponentList::Remove(ComponentList::IterableComponents& components)
{
	for (auto component : components) {
		Remove(component);
	}
}

void ComponentList::Update()
{
	LockMode(ComponentList::LockModes::Locked);
	for (auto component : m_components) {
		if (component->m_active) {
			component->Update();
		}
	}
	LockMode(ComponentList::LockModes::Open);
}

void ComponentList::Render()
{
	LockMode(ComponentList::LockModes::Error);
	for (auto component : m_components) {
		if (component->m_visible) {
			component->Render();
		}
	}
	LockMode(ComponentList::LockModes::Open);
}

void ComponentList::DebugRender(Camera* camera)
{
	LockMode(ComponentList::LockModes::Error);
	for (auto component : m_components) {
		component->DebugRender(camera);
	}
	LockMode(ComponentList::LockModes::Open);
}

void ComponentList::HandleGraphicsReset()
{
    LockMode(LockModes::Error);
    for (auto component : m_components) {
        component->HandleGraphicsReset();
    }
    LockMode(LockModes::Open);
}

void ComponentList::HandleGraphicsCreate()
{
    LockMode(LockModes::Error);
    for (auto component : m_components) {
        component->HandleGraphicsCreate();
    }
    LockMode(LockModes::Open);
}