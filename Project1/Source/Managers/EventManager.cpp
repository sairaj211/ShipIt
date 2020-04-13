#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "EventManager.h"

#include "MemoryManager.h"
#include "ComponentManager.h"
#include "EntityManager.h"

#include "Component.h"
#include "Entity.h"
EventManager* EventManager::mEventManager = nullptr;

EventManager::EventManager()
{
	mTimedEvents = std::list<Event*>();
	mUsedEvents = std::unordered_set<Event*>();
}


EventManager::~EventManager()
{
	for (auto e : mTimedEvents) {
		MemoryManager::GetInstance()->FreeEvent(e);
	}
	mTimedEvents.clear();
	Clean();
}

void EventManager::BroadcastEvent(Event * _event)
{
	std::vector<Component*> components = ComponentManager::GetInstance()->GetAllComponents();
	int size = int(components.size());
	for (int index = 0; index < size; ++index)
		components[index]->HandleEvent(_event);

	mUsedEvents.insert(_event);
}

void EventManager::AddTimedEvent(Event * _event)
{
	mTimedEvents.push_back(_event);
}

void EventManager::Update(float dt)
{
	//loop through events and decrease their timers
	std::list<Event*>::iterator it, itTmp;
	it = mTimedEvents.begin();
	float _dt = dt * .001f;
	while (it != mTimedEvents.end()) {
		Event* _event = *it;
		_event->SetTime( _event->GetTime() - _dt);
		if (_event->GetTime() <= 0.0f) {
			BroadcastEvent(_event);
			it = mTimedEvents.erase(it);
		}
		else {
			++it;
		}
	}
}

void EventManager::Subscribe(std::string _type, int id)
{
	std::unordered_map<std::string, std::list<int>>::iterator found = mSubs.find(_type);
	if (found == mSubs.end()) {
		std::list<int> subs;
		subs.push_back(id);
		mSubs[_type] = subs;
	}
	else {
		std::list<int> subs = mSubs[_type];
		subs.push_back(id);
		mSubs[_type] = subs;
	}
}

void EventManager::BroadCastToSubs(Event * _event)
{
	auto subList = mSubs[_event->GetType()];
	for (auto sub : subList) {
		SendEvent(_event, sub);	
	}
}

void EventManager::SendEvent(Event * _event, int id)
{
	mUsedEvents.insert(_event);
	ComponentManager* componentManager = ComponentManager::GetInstance();
	std::vector<Component*> components = componentManager->GetComponents(id);
	int components_size = int(components.size());

	for (int comp_index = 0; comp_index < components_size; ++comp_index) {
		components[comp_index]->HandleEvent(_event);
	}
}


void EventManager::Init()
{
	LOG_INFO("EventManager Init");
}

void EventManager::CleanUp()
{
	delete EventManager::mEventManager;
}

void EventManager::SendEventToScene(Event * apEvent)
{
}

void EventManager::Clean()
{
	for (auto e : mUsedEvents) 
		MemoryManager::GetInstance()->FreeEvent(e);
	mUsedEvents.clear();
	
}

