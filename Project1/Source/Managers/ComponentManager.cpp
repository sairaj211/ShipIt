#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ComponentManager.h"
#include "Component.h"
#include "Components/Transform.h"
#include "EntityManager.h"
#include "Entity.h"
#include "ResourceManager.h"
#include "Resources/JsonData.h"
#include "MemoryManager.h"
#include "SceneManager.h"

ComponentManager* ComponentManager::mInstance = nullptr;

ComponentManager::ComponentManager()
{

}

ComponentManager::~ComponentManager()
{

}

ComponentManager* ComponentManager::GetInstance()
{
	if (!mInstance)
		mInstance = new ComponentManager();

	return mInstance;
}

void ComponentManager::Update(float a_deltaTime, bool a_ignoreEnabled)
{
	std::list<int> ids = SCENE_MANAGER->PeekScene();
	for (auto& element : mEnabledComponentMap) {
		// Loop through each component of a given type and update it
		for (Component* pComponent : element.second) {
			if(std::find(ids.begin(),ids.end(),   pComponent->GetOwnerID()) != ids.end())
				pComponent->Update(a_deltaTime);
		}
	}

	if (a_ignoreEnabled) {
		for (auto& element : mDisabledComponentMap) {
			// Loop through each component of a given type and update it
			for (Component* pComponent : element.second)
				if (std::find(ids.begin(), ids.end(), pComponent->GetOwnerID()) != ids.end())
					pComponent->Update(a_deltaTime);
		}
	}
}

Component* ComponentManager::AddComponent(int a_entityID, std::string a_type)
{
	Entity* pEntityOwner = ENTITY_MANAGER->GetEntity(a_entityID, true);
	Component* pNewComponent = nullptr;

	// Check if the specified entity owner exists
	if (pEntityOwner) {
		// Add new component to the given entity
		pNewComponent = InitializeComponent(pEntityOwner, a_type);
		// Add new component to component maps
		if (pEntityOwner->IsEnabled()) {
			mEnabledComponentMap[a_type].push_back(pNewComponent);
			mEnabledEntityComponentMap[a_entityID].push_back(pNewComponent);
		}
		else {
			mDisabledComponentMap[a_type].push_back(pNewComponent);
			mDisabledEntityComponentMap[a_entityID].push_back(pNewComponent);
		}
	}

	return pNewComponent;
}

Component* ComponentManager::AddRuntimeComponent(int a_entityID, std::string a_type)
{
	Component* pNewComponent = nullptr;

	// Get the entity from the list of runtime entities if it doesn't already exist
	Entity* pEntityOwner = ENTITY_MANAGER->GetEntity(a_entityID, true);
	if (!pEntityOwner)
		pEntityOwner = ENTITY_MANAGER->GetRuntimeEntity(a_entityID);
	
	// Check if the specified entity owner exists
	if (pEntityOwner) {
		// Add new component to the given entity
		Component* pNewComponent = InitializeComponent(pEntityOwner, a_type);
		// Add new component to the create list
		mComponentCreateList.push_back(pNewComponent);
	}

	return pNewComponent;
}

void ComponentManager::ClearComponents(int a_entityID)
{
	// Get the list of the specified entity's components
	std::vector<Component*> entityComponents = GetComponents(a_entityID, true);

	// Add each of the specified entity's components to the delete list
	for (Component* pComponent : entityComponents)
		mComponentDeleteList.push_back(pComponent);
}

std::vector<Component*> ComponentManager::GetComponents(int a_entityID, bool a_ignoreEnabled)
{
	std::vector<Component*> components = mEnabledEntityComponentMap[a_entityID];
	if (a_ignoreEnabled && components.empty())
		components = mDisabledEntityComponentMap[a_entityID];

	return components;
}

std::vector<Component*> ComponentManager::GetRuntimeComponents(int a_entityID)
{
	std::vector<Component*> components;

	for (Component* pComponent : mComponentCreateList)
		if (pComponent->GetOwnerID() == a_entityID)
			components.push_back(pComponent);

	return components;
}

std::vector<Component*> ComponentManager::GetAllRuntimeComponents()
{
	return mComponentCreateList;
}

std::vector<Component*> ComponentManager::GetAllComponents(bool a_ignoreEnabled)
{
	std::vector<Component*> components;

	for (auto element : mEnabledComponentMap)
		components.insert(components.end(), element.second.begin(), element.second.end());
	
	if (a_ignoreEnabled)
		for (auto element : mDisabledComponentMap)
			components.insert(components.end(), element.second.begin(), element.second.end());

	return components;
}

void ComponentManager::SetComponentsEnabled(int a_entityID, bool a_isEnabled)
{
	// Check if specified Entity is valid
	Entity* pEntity = ENTITY_MANAGER->GetEntity(a_entityID, true);
	if (!pEntity)
		return;

	// Check if Entity isn't already enabled/disabled
	if (pEntity->IsEnabled() != a_isEnabled) {
		ENTITY_MANAGER->EntityEnable(a_entityID, a_isEnabled);
		return;
	}

	// TODO: This logic doesn't work if the component was already enabled/disabled
	if (a_isEnabled) {
		std::vector<Component*> entityComponents = mDisabledEntityComponentMap[a_entityID];

		// Update EntityComponent Maps
		mEnabledEntityComponentMap[a_entityID] = entityComponents;
		mDisabledEntityComponentMap.erase(a_entityID);

		// Update TypeComponent Maps
		for (Component* pComponent : entityComponents) {
			mEnabledComponentMap[pComponent->GetType()].push_back(pComponent);
			std::vector<Component*>* pComponentTypeList = &mDisabledComponentMap[pComponent->GetType()];
			pComponentTypeList->erase(find(pComponentTypeList->begin(), pComponentTypeList->end(), pComponent));
			if (pComponentTypeList->size() == 0)
				mDisabledComponentMap.erase(pComponent->GetType());
		}
	}
	else {
		std::vector<Component*> entityComponents = mEnabledEntityComponentMap[a_entityID];

		// Update EntityComponent Maps
		mDisabledEntityComponentMap[a_entityID] = entityComponents;
		mEnabledEntityComponentMap.erase(a_entityID);

		// Update TypeComponent Maps
		for (Component* pComponent : entityComponents) {
			mDisabledComponentMap[pComponent->GetType()].push_back(pComponent);
			std::vector<Component*>* pComponentTypeList = &mEnabledComponentMap[pComponent->GetType()];
			pComponentTypeList->erase(find(pComponentTypeList->begin(), pComponentTypeList->end(), pComponent));
			if (pComponentTypeList->size() == 0)
				mEnabledComponentMap.erase(pComponent->GetType());
		}
	}
}

unsigned int ComponentManager::GetNumTypes(bool a_ignoreEnabled) {
	int numTypes = int(mEnabledComponentMap.size());
	if (a_ignoreEnabled)
		for (auto& element : mDisabledComponentMap)
			if (mEnabledComponentMap.find(element.first) == mEnabledComponentMap.end())
				++numTypes;
	
	return numTypes;
}

std::vector<Component*> ComponentManager::GetComponents(std::string a_type, bool a_ignoreEnabled)
{
	std::vector<Component*> components = mEnabledComponentMap[a_type];
	if (a_ignoreEnabled) {
		std::vector<Component*> disabledComponents = mDisabledComponentMap[a_type];
		components.insert(components.begin(), disabledComponents.begin(), disabledComponents.end());
	}

	return components;
}

void ComponentManager::ClearRuntimeLists()
{
	// Delete components in the delete list
	for (Component* pDeleteComponent : mComponentDeleteList) {
		// Check if the component has already been deleted
		if (pDeleteComponent->GetOwnerID() != 0) {
			// Free component and remove it from the enabled component maps
			std::vector<Component*>* pComponentTypeList = &mEnabledComponentMap[pDeleteComponent->GetType()];
			std::vector<Component*>* pEntityComponentList = &mEnabledEntityComponentMap[pDeleteComponent->GetOwnerID()];
			auto enabledTypeIt = find(pComponentTypeList->begin(), pComponentTypeList->end(), pDeleteComponent);
			if (enabledTypeIt != pComponentTypeList->end())
				pComponentTypeList->erase(enabledTypeIt);
			if (pComponentTypeList->size() == 0)
				mEnabledComponentMap.erase(pDeleteComponent->GetType());

			auto enabledEntityIt = find(pEntityComponentList->begin(), pEntityComponentList->end(), pDeleteComponent);
			if (enabledEntityIt != pEntityComponentList->end())
				pEntityComponentList->erase(enabledEntityIt);
			if (pEntityComponentList->size() == 0)
				mEnabledEntityComponentMap.erase(pDeleteComponent->GetOwnerID());

			// Free component and remove it from the disabled component maps
			pComponentTypeList = &mDisabledComponentMap[pDeleteComponent->GetType()];
			pEntityComponentList = &mDisabledEntityComponentMap[pDeleteComponent->GetOwnerID()];
			auto disabledTypeIt = find(pComponentTypeList->begin(), pComponentTypeList->end(), pDeleteComponent);
			if (disabledTypeIt != pComponentTypeList->end())
				pComponentTypeList->erase(disabledTypeIt);
			if (pComponentTypeList->size() == 0)
				mDisabledComponentMap.erase(pDeleteComponent->GetType());
			
			auto disabledEntityIt = find(pEntityComponentList->begin(), pEntityComponentList->end(), pDeleteComponent);
			if (disabledEntityIt != pEntityComponentList->end())
				pEntityComponentList->erase(disabledEntityIt);
			if (pEntityComponentList->size() == 0)
				mDisabledEntityComponentMap.erase(pDeleteComponent->GetOwnerID());

			MemoryManager::GetInstance()->FreeComponent(pDeleteComponent);
		}
	}
	mComponentDeleteList.clear();

	// Add runtime components to the component maps
	for (Component* pNewComponent : mComponentCreateList) {
		if (ENTITY_MANAGER->GetEntity(pNewComponent->GetOwnerID(), true)->IsEnabled()) {
			mEnabledComponentMap[pNewComponent->GetType()].push_back(pNewComponent);
			mEnabledEntityComponentMap[pNewComponent->GetOwnerID()].push_back(pNewComponent);
		}
		else {
			mDisabledComponentMap[pNewComponent->GetType()].push_back(pNewComponent);
			mDisabledEntityComponentMap[pNewComponent->GetOwnerID()].push_back(pNewComponent);
		}
	}
	mComponentCreateList.clear();
}

void ComponentManager::Cleanup()
{
	delete mInstance;
	mInstance = nullptr;
}

Component* ComponentManager::InitializeComponent(Entity* a_pEntityOwner, std::string a_type)
{
	//Create and initialize new component
	Component* pNewComponent = MemoryManager::GetInstance()->GetComponentFromMemory(a_type);
	pNewComponent->SetOwnerID(a_pEntityOwner->GetID());
	// Serialize component
	std::string datafile = a_pEntityOwner->GetDataFile();
	pNewComponent->Serialize(ResourceManager::GetInstance()->LoadJsonData(datafile)->mRoot[a_type]);

	return pNewComponent;
}