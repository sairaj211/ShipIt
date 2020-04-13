#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Carrier.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Entity.h"
#include "Events/OrphanEvent.h"
#include "Resources/JsonData.h"

Carrier::Carrier() : Component("Carrier")
{

}

Carrier::~Carrier()
{

}

void Carrier::Reset()
{
	mItems.clear();
	mStartingItemMap.clear();

	Component::Reset();
}

void Carrier::Init()
{
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);

	std::vector<int> children = pEntity->GetChildIDs();
	// Get the Entity IDs of the specified children
	for (std::pair<int, int>& indexID : mStartingItemMap) {
		int index = indexID.first;
		if (index >= 0 && index < children.size()) {
			indexID.second = children[index];
			mItems.push_back(children[index]);
		}
	}
}

Carrier* Carrier::Create()
{
	return new Carrier();
}

void Carrier::Update(float a_deltaTime)
{
	// Literally Nothing
}

void Carrier::DisplayDebug()
{
	// TODO: Expose starting items to ImGui

	// Display the IDs of the items being held
	ImGui::Text("Item IDs:");
	for (int i = 0; i < mItems.size(); ++i) {
		ImGui::Text("	Item %d: %d", i, mItems[i]);
	}
}

void Carrier::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["ItemIndices"];
	if (!dataRoot.isNull()) {
		mStartingItemMap.clear();
		for (unsigned int i = 0; i < dataRoot.size(); ++i) {
			mStartingItemMap.push_back(std::pair<int, int>(dataRoot[i].asInt(), -1));
		}
	}
}

void Carrier::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	std::string dataFile = pEntity->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Carrier"];

	std::vector<int> children = pEntity->GetChildIDs();

	// TODO: Implement Child deserialization
	bool isDifferent = mStartingItemMap.size() != mItems.size();
	if (!isDifferent) {
		for (unsigned int i = 0; i < mStartingItemMap.size(); ++i) {
			std::pair<int, int> indexID = mStartingItemMap[i];
			if (find(children.begin(), children.end(), indexID.second) != find(children.begin(), children.end(), mItems[i])) {
				isDifferent = true;
				break;
			}
		}
	}

	if (isDifferent || a_ignoreDefault) {
		Json::Value indices(Json::arrayValue);
		for (int i = 0; i < children.size(); ++i) {
			if (find(mItems.begin(), mItems.end(), children[i]) != mItems.end()) {
				indices.append(i);
			}
		}
		a_root["ItemIndices"] = indices;
	}
}

void Carrier::HandleEvent(Event* a_pEvent)
{
	if (a_pEvent->GetType() == "OrphanEvent") {
		OrphanEvent* pOrphanEvent = static_cast<OrphanEvent*>(a_pEvent);

		// Remove the removed child from the list of items
		if (pOrphanEvent->mParentID == mOwnerID) {
			auto orphanedChild = find(mItems.begin(), mItems.end(), pOrphanEvent->mChildID);
			if (orphanedChild != mItems.end())
				mItems.erase(orphanedChild);
		}
	}
}

void Carrier::AddItem(int a_itemID)
{
	Entity* pChild = ENTITY_MANAGER->GetEntity(a_itemID);
	if (!pChild)
		pChild = ENTITY_MANAGER->GetRuntimeEntity(a_itemID);

	if (pChild) {
		if (!IsItem(a_itemID)) {
			mItems.push_back(a_itemID);
			pChild->SetParentID(mOwnerID);
		}
	}
}

void Carrier::RemoveItem(int a_itemID)
{
	auto removedItem = find(mItems.begin(), mItems.end(), a_itemID);
	if (removedItem != mItems.end()) {
		Entity* pChild = ENTITY_MANAGER->GetEntity(a_itemID);
		if (!pChild)
			pChild = ENTITY_MANAGER->GetRuntimeEntity(a_itemID);

		pChild->SetParentID(-1);
	}
}

void Carrier::RemoveItems(std::string a_type)
{
	std::vector<int> removeItems = GetItems(a_type);
	for (int i = 0; i < removeItems.size(); ++i)
		RemoveItem(removeItems[i]);
}

void Carrier::Void()
{
	for (int i = 0; i < mItems.size(); ++i) {
		Entity* pChild = ENTITY_MANAGER->GetEntity(mItems[i]);
		if (!pChild)
			pChild = ENTITY_MANAGER->GetRuntimeEntity(mItems[i]);

		if (pChild)
			pChild->SetParentID(-1);
	}
	mItems.clear();
}

bool Carrier::IsItem(int a_itemID)
{
	return find(mItems.begin(), mItems.end(), a_itemID) != mItems.end();
}

std::vector<int> Carrier::GetItems()
{
	return mItems;
}

std::vector<int> Carrier::GetItems(std::string a_type)
{
	std::vector<int> typeItems;
	for (int i = 0; i < mItems.size(); ++i) {
		Entity* pChild = ENTITY_MANAGER->GetEntity(mItems[i]);
		if (!pChild)
			pChild = ENTITY_MANAGER->GetRuntimeEntity(mItems[i]);

		if (pChild) {
			if (pChild->GetType() == a_type)
				typeItems.push_back(mItems[i]);
		}
	}

	return typeItems;
}

GEAR_API void Carrier::Clone(int objID)
{
	//TODO
	Carrier *rhs = static_cast<Carrier*>(COMPONENT_MANAGER->GetComponent<Carrier>(objID, "Carrier"));

}
