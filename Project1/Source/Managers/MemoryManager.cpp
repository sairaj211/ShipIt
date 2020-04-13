#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "MemoryManager.h"


MemoryManager* MemoryManager::mInstance = nullptr;

MemoryManager::MemoryManager() {

}

MemoryManager::~MemoryManager() {
	for (auto go : mEntityMemory)
	{
		delete go;
	}
	mEntityMemory.clear();

	for (auto com : mComponentMemory)
	{
		for (auto comp : com.second)
		{
			delete comp;
		}
		com.second.clear();
	}
	mComponentMemory.clear();


	for (auto evt_kv : mEventMemory)
	{
		for (auto evt : evt_kv.second)
		{
			delete evt;
		}
		evt_kv.second.clear();
	}
	mEventMemory.clear();
}

MemoryManager* MemoryManager::GetInstance() {
	if (!mInstance) {
		mInstance = new MemoryManager();
	}
	return mInstance;
}

void MemoryManager::Init()
{
	LOG_INFO("Memory Init"); 
	mNumberofGameObjects = 2048;
	mNumberofComponents = 2048;
	mNumberofEvents = 1024;


	// Register Components Below
	REGISTER_COMPONENT(Transform);
	REGISTER_COMPONENT(Body);
	REGISTER_COMPONENT(Collider);
	REGISTER_COMPONENT(ModelComponent);
	REGISTER_COMPONENT(LocalLightComponent);
	REGISTER_COMPONENT(DRLightComponent);
	REGISTER_COMPONENT(SpotLight);
	REGISTER_COMPONENT(Controller);
	REGISTER_COMPONENT(ScriptComponent);
	REGISTER_COMPONENT(ParticlesComponent);
	REGISTER_COMPONENT(Animator);
	REGISTER_COMPONENT(Player);
	REGISTER_COMPONENT(Audio);
	REGISTER_COMPONENT(ButtonComponent);
	REGISTER_COMPONENT(Camera);
	REGISTER_COMPONENT(Observer);
	REGISTER_COMPONENT(AreaSearch);
	REGISTER_COMPONENT(Carrier);
	REGISTER_COMPONENT(Processor);
	REGISTER_COMPONENT(Grid);
	REGISTER_COMPONENT(GridWaypoint);
	REGISTER_COMPONENT(Bullet);
	REGISTER_COMPONENT(Enemy);
	REGISTER_COMPONENT(Wall);
	REGISTER_COMPONENT(UIComponent);
	REGISTER_COMPONENT(AffineSequence);
	REGISTER_COMPONENT(Block);
	REGISTER_COMPONENT(Teleport);
	REGISTER_COMPONENT(LevelButton);
	REGISTER_COMPONENT(Building);

	//Register Events Below
	REGISTER_EVENT(UpdatedScriptEvent);
	REGISTER_EVENT(PauseEvent);
	REGISTER_EVENT(ContactEvent);
	REGISTER_EVENT(AttackEvent);
	REGISTER_EVENT(OrphanEvent);
	REGISTER_EVENT(ResetEntityEvent);
	REGISTER_EVENT(RecipeMatchEvent);
	REGISTER_EVENT(PlayerPickupEvent);
	REGISTER_EVENT(PlayerDropEvent);
	REGISTER_EVENT(ContainerInputEvent);
	REGISTER_EVENT(ContainerOutputEvent);
	REGISTER_EVENT(ContainerMatchEvent);
	REGISTER_EVENT(RequestFilledEvent);
	REGISTER_EVENT(RequestMadeEvent);
	REGISTER_EVENT(GeneratorOutputEvent);
	REGISTER_EVENT(GeneratorInputEvent);
	REGISTER_EVENT(InputEvent);
	REGISTER_EVENT(SetCellEvent);
	REGISTER_EVENT(CellUpdateEvent);
	REGISTER_EVENT(CellEjectEvent);
	REGISTER_EVENT(CreateGameEvent);
	REGISTER_EVENT(InitLevelEvent);
	REGISTER_EVENT(UpdateLevelEvent);
	REGISTER_EVENT(UIActionEvent);
	REGISTER_EVENT(UpdateSliderPositionEvent);
	REGISTER_EVENT(ChangeAnimationStateEvent);
	REGISTER_EVENT(HighlightEvent);
	REGISTER_EVENT(LevelCompleteEvent);

	REGISTER_EVENT(ResetGridEvent);
	//----------------------------------------------------------------------------------------------------------------------------------------------------//


	for (unsigned int i = 0; i < mNumberofGameObjects; ++i)
	{
		Entity* temp = new Entity();
		mEntityMemory.push_back(temp);
	}

	std::unordered_map<std::string, Component*>::iterator it = mComponentMap.begin();

	while (it != mComponentMap.end())
	{
		// TODO:
		std::string compType = it->second->GetType();
		if (compType == "BulletComp" || compType == "Wall"|| compType == "Enemy" || compType == "Building" || compType == "Camera") {
			for (size_t i = 0; i < 20; ++i)
				mComponentMemory[compType].push_back(it->second->Create());
		}
		else {
			for (size_t i = 0; i < mNumberofComponents; ++i)
				mComponentMemory[compType].push_back(it->second->Create());
		}
		it++;
	}

	std::unordered_map<std::string, Event*>::iterator event_it = mEventMap.begin();

	while (event_it != mEventMap.end())
	{
		for (size_t i = 0; i < mNumberofEvents; ++i)
				mEventMemory[event_it->first].push_back(event_it->second->Create());
		event_it++;
	}

}

void MemoryManager::Cleanup() {
	delete mInstance;
}

Entity * MemoryManager::GetEntityFromMemory()
{
	if (mEntityMemory.empty())
	{
		LOG_INFO("[MEMORY] No GameObjects to Allocate");
	}

	Entity* NewEntity = mEntityMemory.front();
	mEntityMemory.pop_front();
	return NewEntity;

}

Component * MemoryManager::GetComponentFromMemory(std::string componentType)
{
	if (mComponentMemory[componentType].empty())
	{

		LOG_INFO("[MEMORY] No Components to Allocate");

		return nullptr;
	}
	else {
		Component* NewComp = mComponentMemory[componentType].front();
		mComponentMemory[componentType].pop_front();
		return NewComp;
	}
}

Event * MemoryManager::GetEventFromMemory(std::string _type)
{

	if (mEventMemory[_type].empty())
	{
		LOG_INFO("[MEMORY] No Events to Allocate");
		LOG_INFO(_type);
	}

	Event* newEvent = mEventMemory[_type].front();
	mEventMemory[_type].pop_front();

	return newEvent;
}

template <typename T>
void MemoryManager::RegisterComponent(const char* componentName)
{
	//check for duplicates
	typename std::unordered_map<std::string, Component*>::iterator it = mComponentMap.find(componentName);

	if (it == mComponentMap.end())
	{
		T* component = new T();
		mComponentMap.insert({ componentName , component });
	}
	else
	{
		std::cout << "Component already registered\n";
	}
}


template <typename T>
void MemoryManager::RegisterEvent(std::string _type)
{
	//check for duplicates
	typename std::unordered_map<std::string, Event*>::iterator it = mEventMap.find(_type);

	if (it == mEventMap.end())
	{
		T* newEvent = new T();
		mEventMap.insert({ _type , newEvent });
	}
	else
	{
		std::cout << "Event already registered\n";
	}
}

void MemoryManager::FreeEntity(Entity *freeEntity)
{
	freeEntity->Reset();
	mEntityMemory.push_back(freeEntity);
}

void MemoryManager::FreeComponent(Component *freeComponent)
{
	freeComponent->Reset();

	std::string abc = freeComponent->GetType();

	mComponentMemory[abc].push_back(freeComponent);
}

void MemoryManager::FreeEvent(Event *freeEvent)
{
	freeEvent->Reset();

	mEventMemory[freeEvent->GetType()].push_back(freeEvent);
}

