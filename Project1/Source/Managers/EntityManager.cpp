#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "EntityManager.h"
#include "Entity.h"
#include "MemoryManager.h"
#include "ResourceManager.h"
#include "Resources/JsonData.h"
#include "ComponentManager.h"
#include "SceneManager.h"
#include "Component.h"
#include "Engine.h"	
#include "Editor/LevelEditor.h"

EntityManager* EntityManager::mInstance = nullptr;

EntityManager::EntityManager()
{
	mMasterID = 1;
}

EntityManager::~EntityManager()
{

}

EntityManager* EntityManager::GetInstance()
{
	if (!mInstance)
		mInstance = new EntityManager();
	
	return mInstance;
}

Entity* EntityManager::GetEntity(int a_entityID, bool a_ignoreEnabled)
{
	Entity* pEntity = mEntityMap[a_entityID];

	// Return the specified entity if it exists and is enabled
	if (pEntity)
		if (pEntity->IsEnabled() || a_ignoreEnabled)
			return pEntity;

	return nullptr;
}

Entity* EntityManager::GetRuntimeEntity(int a_entityID)
{
	// Search for an entity in the entity create list that matches the given ID
	for (Entity* pCreateEntity : mEntityCreateList)
		if (pCreateEntity->GetID() == a_entityID)
			return pCreateEntity;
	
	return nullptr;
}

std::vector<Entity*> EntityManager::GetEntities(std::string a_type, bool a_ignoreEnabled)
{
	std::vector<Entity*> entityTypeMaster = mEntityTypeMap[a_type];
	std::vector<Entity*> entities;

	if (!a_ignoreEnabled) {
		for (Entity* pEntity : entityTypeMaster) {
			if (pEntity->IsEnabled()) {
				entities.push_back(pEntity);
			}
		}
	}
	else {
		entities = entityTypeMaster;
	}

	return entities;
}

Entity* EntityManager::CreateEntity(std::string a_fileName, int a_parentID,std::string a_scene)
{
	// Create new entity described in given file
	Entity* pNewEntity = InitializeNewEntity(a_fileName);

	// Add entity to the active entity map
	mEntityMap[pNewEntity->GetID()] = pNewEntity;

	// Serialize new Entity
	pNewEntity->Serialize(ResourceManager::GetInstance()->LoadJsonData(a_fileName, "./Resources/Prefab/")->mRoot, false);
	mEntityTypeMap[pNewEntity->GetType()].push_back(pNewEntity); // This dependency is weird...

	// Initialize Components
	InitializeNewComponents(pNewEntity, false);
	pNewEntity->SetParentID(a_parentID);

	return pNewEntity;
}

Entity* EntityManager::CreateRuntimeEntity(std::string a_fileName, int a_parentID, std::string a_scene)
{
	// Create new entity described in given file
	Entity* pNewEntity = InitializeNewEntity(a_fileName);
	// Add new entity to the create list
	mEntityCreateList.push_back(pNewEntity);

	// Serialize new Entity
	pNewEntity->Serialize(ResourceManager::GetInstance()->LoadJsonData(a_fileName, "./Resources/Prefab/")->mRoot, true);

	// Initialize Components
	InitializeNewComponents(pNewEntity, true);
	pNewEntity->SetParentID(a_parentID);

	LEVEL_EDITOR->mRunTimeTimeList.push_back(pNewEntity->GetID());

	return pNewEntity;
}

void EntityManager::DeleteEntity(int a_entityID)
{
	Entity* pDeleteEntity = GetEntity(a_entityID, true);

	// Check that the entity exists
	if (pDeleteEntity) {
		// Add corresponding entity to the delete list

		mEntityDeleteList.push_back(pDeleteEntity);
		std::vector <int> children = pDeleteEntity->GetChildIDs();
		for (int  child : children)
		{

			DeleteEntity(child);
		}
 
	}
}

void EntityManager::EntityEnable(int a_entityID, bool a_enable)
{
	Entity* pEntity = GetEntity(a_entityID, true);
	if (!pEntity)
		pEntity = GetRuntimeEntity(a_entityID);

	// Check that the entity exists
	if (pEntity) {
		mEntityEnableList.push_back(std::pair<Entity*, bool>(pEntity, a_enable));

		// Toggle child Entities
		std::vector<int> children = pEntity->GetChildIDs();
		for (int i = 0; i < children.size(); ++i) {
			EntityEnable(children[i], a_enable);
		}
	}
}

void EntityManager::ClearRuntimeLists()
{
	// Delete entities in the delete list
	for (Entity* pDeleteEntity : mEntityDeleteList) {
		// Check if the entity has already been deleted
		if (pDeleteEntity->GetID() != 0) {
			// Free corresponding components
			ComponentManager::GetInstance()->ClearComponents(pDeleteEntity->GetID());

			// Remove Entity from its parent Entity's child list
			pDeleteEntity->SetParentID(-1);

			// Free entity and remove it from the entity maps
			std::vector<Entity*>* pEntityTypeList = &mEntityTypeMap[pDeleteEntity->GetType()];
			pEntityTypeList->erase(find(pEntityTypeList->begin(), pEntityTypeList->end(), pDeleteEntity));
			if (pEntityTypeList->size() == 0)
				mEntityTypeMap.erase(pDeleteEntity->GetType());
			mEntityMap.erase(pDeleteEntity->GetID());
			SCENE_MANAGER->RemoveEntity(pDeleteEntity->GetID());
			MemoryManager::GetInstance()->FreeEntity(pDeleteEntity);
		}
	}
	mEntityDeleteList.clear();

	// Add runtime entities to the lists of active entities
	for (Entity* pNewEntity : mEntityCreateList) {
		mEntityTypeMap[pNewEntity->GetType()].push_back(pNewEntity);
		mEntityMap[pNewEntity->GetID()] = pNewEntity;
		
		SCENE_MANAGER->AddEntity(pNewEntity->GetID());
	}
	mEntityCreateList.clear();

	// Toggle entities IsEnabled flag
	for (std::pair<Entity*, bool> pEnableEntityPair : mEntityEnableList) {
		if (pEnableEntityPair.first->IsEnabled() != pEnableEntityPair.second) {
			pEnableEntityPair.first->SetEnabled(pEnableEntityPair.second);
			COMPONENT_MANAGER->SetComponentsEnabled(pEnableEntityPair.first->GetID(), pEnableEntityPair.first->IsEnabled());
		}
	}
	mEntityEnableList.clear();
}

void EntityManager::Cleanup()
{
	delete mInstance;
	mInstance = nullptr;
}

int EntityManager::ReturnID()
{
	return int(mEntityMap.size());
}


Entity* EntityManager::InitializeNewEntity(std::string a_fileName)
{
	// Initialize new entity described in given file
	Entity* pNewEntity = MemoryManager::GetInstance()->GetEntityFromMemory();
	pNewEntity->SetDataFile(a_fileName);
	pNewEntity->SetID(mMasterID);
	++mMasterID;

	return pNewEntity;
}

void EntityManager::InitializeNewComponents(Entity* a_pNewEntity, bool a_isRuntime)
{
	// Load components
	Json::Value defaultEntityRoot = ResourceManager::GetInstance()->LoadJsonData(a_pNewEntity->GetDataFile(), "./Resources/Prefab/")->mRoot;
	Json::Value::Members componentNames = defaultEntityRoot.getMemberNames();
	Component* pNewComponent = nullptr;
	for (size_t j = 0; j < defaultEntityRoot.size(); ++j) {
		// If this json member is not the type specifier, load and reserialize the corresponding component
		if (componentNames[j] != "Type" && componentNames[j] != "IsEnabled" && componentNames[j] != "Name" && componentNames[j] != "Children") {
			if (!a_isRuntime)
				pNewComponent = ComponentManager::GetInstance()->AddComponent(a_pNewEntity->GetID(), componentNames[j]);
			else
				pNewComponent = ComponentManager::GetInstance()->AddRuntimeComponent(a_pNewEntity->GetID(), componentNames[j]);
		}
	}
	if (a_isRuntime)
	{
		std::vector<Component*>  components = COMPONENT_MANAGER->GetRuntimeComponents(a_pNewEntity->GetID());
		for (auto& com : components)
			com->Init();
	}
}