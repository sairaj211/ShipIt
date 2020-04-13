#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Entity.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
#include "Managers/ComponentManager.h"
#include "Managers/EntityManager.h"
#include "Managers/ObjectFactory.h"
#include "Managers/EventManager.h"
#include "Managers/MemoryManager.h"
#include "Events/OrphanEvent.h"
#include "Component.h"
#include "Engine.h"
#include "Editor/LevelEditor.h"

Entity::Entity()
{
	mType = "Empty";
	mID = 0;
	mDataFile = "Default";
	mIsEnabled = true;
	mName = "Entity";
	mParentID = -1;
}

Entity::~Entity()
{

}

void Entity::Reset()
{
//	mType = "Empty";
	mID = 0;
	mDataFile = "Default";
	mIsEnabled = true;
	mName = "Entity";
	mParentID = -1;
	mChildIDs.clear();
}

void Entity::SetType(std::string a_type)
{
	mType = a_type;
}

std::string Entity::GetType()
{
	return mType;
}

void Entity::SetID(int a_id)
{
	mID = a_id;
}

int Entity::GetID()
{
	return mID;
}

void Entity::SetDataFile(std::string a_fileName)
{
	mDataFile = a_fileName;
}

std::string Entity::GetDataFile()
{
	return mDataFile;
}

void Entity::SetEnabled(bool a_enable)
{
	mIsEnabled = a_enable;
}

void Entity::SetAllEnabled(bool a_enable)
{
	if (mIsEnabled != a_enable) {
		SetEnabled(a_enable);
		COMPONENT_MANAGER->SetComponentsEnabled(mID, a_enable);

		for (int i = 0; i < mChildIDs.size(); ++i) {
			Entity* pChild = ENTITY_MANAGER->GetEntity(mChildIDs[i], true);
			if (pChild) {
				pChild->SetAllEnabled(a_enable);
			}
		}
	}
}

bool Entity::IsEnabled()
{
	return mIsEnabled;
}

void Entity::SetName(std::string a_name)
{
	mName = a_name;
}

std::string Entity::GetName()
{
	return mName;
}

int Entity::GetParentID()
{
	return mParentID;
}

void Entity::SetParentID(int a_parentID)
{
	if (mParentID != a_parentID) {
		std::vector<Component*> components = COMPONENT_MANAGER->GetComponents(mID, true);
		if (components.empty())
			components = COMPONENT_MANAGER->GetRuntimeComponents(mID);
		Entity* pOldParent = ENTITY_MANAGER->GetEntity(mParentID, true);
		if (!pOldParent)
			pOldParent = ENTITY_MANAGER->GetRuntimeEntity(mParentID);
		if (pOldParent) {
			// Remove this Entity from its old parent
			pOldParent->mChildIDs.erase(find(pOldParent->mChildIDs.begin(), pOldParent->mChildIDs.end(), mID));
			
			for (Component* pComponent : components)
				pComponent->OnRemoveParent();

			// Send OrphanEvent to all Components
			OrphanEvent* pOrphanEvent = static_cast<OrphanEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("OrphanEvent"));
			pOrphanEvent->mParentID = mParentID;
			pOrphanEvent->mChildID = mID;
			EventManager::GetInstance()->BroadcastEvent(pOrphanEvent); // TODO: Optimize
		}

		mParentID = a_parentID;

		Entity* pNewParent = ENTITY_MANAGER->GetEntity(a_parentID, true);
		if (!pNewParent)
			pNewParent = ENTITY_MANAGER->GetRuntimeEntity(a_parentID);
		if (pNewParent) {
			// Add this Entity to its new parent
			pNewParent->mChildIDs.push_back(mID);

			for (Component* pComponent : components)
				pComponent->OnSetParent();
		}

		// If there was an impactful change to the Entity's parent
		if (pOldParent || pNewParent) {
			// Set the dirty flag in each of its Components
			for (Component* pComponent : components)
				pComponent->SetDirty(); // TODO: optimized
		}
	}
}

std::vector<int> Entity::GetChildIDs()
{
	return mChildIDs;
}

std::vector<int> Entity::GetChildIDs(std::string a_type) {
	std::vector<int> childIDsOfType;

	// Add the child IDs to the childIDsOfType vector if their type matches the given type
	for (int childID : mChildIDs) {
		Entity* pChildEntity = ENTITY_MANAGER->GetEntity(childID, true);
		if (!pChildEntity)
			pChildEntity = ENTITY_MANAGER->GetRuntimeEntity(childID);
		if (pChildEntity)
			if (pChildEntity->GetType() == a_type)
				childIDsOfType.push_back(childID);
	}

	return childIDsOfType;
}

void Entity::Serialize(Json::Value a_root, bool a_isRuntime)
{
	Json::Value dataRoot = a_root["Type"];
	if (!dataRoot.isNull())
		mType = dataRoot.asString();

	dataRoot = a_root["Name"];
	if (!dataRoot.isNull())
		mName = dataRoot.asString();
	else if (mName == "Entity")
		mName = mType;

	dataRoot = a_root["Children"];
	if (!dataRoot.isNull()) {
		if (!mChildIDs.empty()) {
			for (int id : mChildIDs) {
				ENTITY_MANAGER->DeleteEntity(id);
			}
		}
		std::vector<int> childIDs = OBJECT_FACTORY->LoadEntities(dataRoot, a_isRuntime, mID);
		for (int ids : childIDs) {
			std::vector<Component*> components;
			if (a_isRuntime)
				components = COMPONENT_MANAGER->GetRuntimeComponents(ids);
			else
				components = COMPONENT_MANAGER->GetComponents(ids, true);

			for (Component* compo : components)
			{

				compo->Init();// May want to init this later
			}
		}
	}

	// This needs to happen after loading children
	dataRoot = a_root["IsEnabled"];
	if (!dataRoot.isNull())
		if (mIsEnabled != dataRoot.asBool())
			ENTITY_MANAGER->EntityEnable(mID, dataRoot.asBool());
}

void Entity::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(mDataFile)->mRoot;

 	if (mType != defaultRoot["Type"].asString() || a_ignoreDefault)
		a_root["Type"] = mType;
 	
 	if (mIsEnabled != defaultRoot["IsEnabled"].asBool() || a_ignoreDefault)
		a_root["IsEnabled"] = mIsEnabled;

	if (mName != defaultRoot["Name"].asString() || a_ignoreDefault)
		a_root["Name"] = mName;

	Json::Value childrenArray(Json::arrayValue);
	for (int i = 0; i < mChildIDs.size(); ++i) {
		Entity* pChild = ENTITY_MANAGER->GetEntity(mChildIDs[i], true);
		
		if (pChild && std::find(LEVEL_EDITOR->mRunTimeTimeList.begin(), LEVEL_EDITOR->mRunTimeTimeList.end(), pChild->GetID()) == LEVEL_EDITOR->mRunTimeTimeList.end()) {
			Json::Value child;
			child["DataFile"] = pChild->GetDataFile();
			pChild->DeSerialize(child, a_ignoreDefault);
			childrenArray.append(child);
		}
	}
	if (childrenArray.size() > 0)
		a_root["Children"] = childrenArray;

	// Deserialize the Components
	std::vector<Component*> components = COMPONENT_MANAGER->GetComponents(mID, true);
	for (Component* pComeponent : components) {
		pComeponent->DeSerialize(a_root[pComeponent->GetType()], a_ignoreDefault);
		if (a_root[pComeponent->GetType()].isNull()) {
			a_root.removeMember(pComeponent->GetType());
		}
	}
}