#include "pch.h"
#include "Managers/Log.h"

#include "Core.h"

#include "AreaSearch.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
#include "Events/ContactEvent.h"
#include "Entity.h"
#include "Collider.h"

Closest::Closest(int a_compareID)
{
	// Note that this will crash if the shape or collider is null
	comparePosition = COMPONENT_MANAGER->GetComponent<Collider>(a_compareID, "Collider")->mpShape->mCenter;
}

bool Closest::operator()(int a_entityID0, int a_entityID1)
{
	glm::vec3 pos0 = COMPONENT_MANAGER->GetComponent<Collider>(a_entityID0, "Collider")->mpShape->mCenter;
	glm::vec3 pos1 = COMPONENT_MANAGER->GetComponent<Collider>(a_entityID1, "Collider")->mpShape->mCenter;
	float dist0 = glm::length(pos0 - comparePosition);
	float dist1 = glm::length(pos1 - comparePosition);
	
	return dist0 < dist1;
}

AreaSearch::AreaSearch() : Component("AreaSearch")
{
	mCompareFunctionsNames.push_back("Closest");
	mCompareFunctionsNames.push_back("None");

	mCompareFunctionName = "None";
}

AreaSearch::~AreaSearch()
{

}

void AreaSearch::Reset()
{
	mFoundIDs.clear();
	mFoundIDsTypeMap.clear();
	mCompareFunctionName = "None";
	mSearchTypes.clear();
	mContactIDs.clear();

	Component::Reset();
}

AreaSearch* AreaSearch::Create()
{
	return new AreaSearch();
}

void AreaSearch::Update(float a_deltaTime)
{
	// Add valid IDs of Entities with relevant types to a refined list of found contacts
	std::vector<int> refinedContacts; // TODO: Optimize
	for (int i = 0; i < mContactIDs.size(); ++i) {
		Entity* pContactEntity = ENTITY_MANAGER->GetEntity(mContactIDs[i]);
		if (pContactEntity)
			if (find(mSearchTypes.begin(), mSearchTypes.end(), pContactEntity->GetType()) != mSearchTypes.end())
				refinedContacts.push_back(mContactIDs[i]);
	}
	
	// Sort the contact IDs based on the current comparison function
	SortContacts(refinedContacts);

	// Add the new sorted Entity IDs to the found lists
	mFoundIDs.clear();
	mFoundIDsTypeMap.clear();
	
	for (int i = 0; i < refinedContacts.size(); ++i) {
		Entity* pContactEntity = ENTITY_MANAGER->GetEntity(refinedContacts[i]);

		mFoundIDsTypeMap[pContactEntity->GetType()].push_back(refinedContacts[i]);
		mFoundIDs.push_back(refinedContacts[i]);
	}
}

void AreaSearch::DisplayDebug()
{
	// List available comparison functions
	if (ImGui::BeginCombo("Current CompareFunction##CurrentCompareFunction", mCompareFunctionName.c_str())) {
		for (int i = 0; i < mCompareFunctionsNames.size(); ++i) {
			bool selected = mCompareFunctionName == mCompareFunctionsNames[i];
			if (ImGui::Selectable(mCompareFunctionsNames[i].c_str(), selected)) {
				mCompareFunctionName = mCompareFunctionsNames[i];
			}
			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// TODO: Expose constraints to ImGui
}

void AreaSearch::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["CompareFunction"];
	if (!dataRoot.isNull())
		mCompareFunctionName = dataRoot.asString();

	dataRoot = a_root["SearchTypes"];
	if (!dataRoot.isNull()) {
		mSearchTypes.clear();
		for (unsigned int i = 0; i < dataRoot.size(); ++i)
			mSearchTypes.push_back(dataRoot[i].asString());
	}
}

void AreaSearch::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["AreaSearch"];

	if (mCompareFunctionName != defaultRoot["CompareFunction"].asString() || a_ignoreDefault)
		a_root["CompareFunction"] = mCompareFunctionName;

	bool isDifferent = defaultRoot["SearchTypes"].size() != mSearchTypes.size();
	if (!isDifferent) {
		for (unsigned int i = 0; i < defaultRoot["SearchTypes"].size(); ++i) {
			if (mSearchTypes[i] != defaultRoot["SearchTypes"][i].asString()) {
				isDifferent = true;
				break;
			}
		}
	}

	if (isDifferent || a_ignoreDefault) {
		Json::Value searchTypes(Json::arrayValue);
		for (int i = 0; i < mSearchTypes.size(); ++i)
			searchTypes.append(mSearchTypes[i]);
		a_root["SearchTypes"] = searchTypes;
	}
}

void AreaSearch::HandleEvent(Event* a_pEvent)
{
	// If the event is a ContactEvent
	if (a_pEvent->GetType() == "ContactEvent") {
		ContactEvent* pContactEvent = static_cast<ContactEvent*>(a_pEvent);
		int otherID = -1;
		if (mOwnerID == pContactEvent->mId1)
			otherID = pContactEvent->mId2;
		if (mOwnerID == pContactEvent->mId2)
			otherID = pContactEvent->mId1;
		if (otherID == -1)
			return;

		std::string otherType = "None";
		Entity* pOtherEntity = ENTITY_MANAGER->GetEntity(otherID);
		if (pOtherEntity)
			otherType = pOtherEntity->GetType();
		// If the type of contact is OnTriggerEnter
		if (pContactEvent->mContactType == OnTriggerEnter) {
			// Add the Entity ID to the list of contact IDs if it is a type we care about
			if (find(mSearchTypes.begin(), mSearchTypes.end(), otherType) != mSearchTypes.end())
				mContactIDs.push_back(otherID);
		}
		// If the type of contact is OnTriggerExit
		else if (pContactEvent->mContactType == OnTriggerExit) {
			// Remove the Entity ID from the ContactIDs vector
			auto contactIt = find(mContactIDs.begin(), mContactIDs.end(), otherID);
			if (contactIt != mContactIDs.end())
				mContactIDs.erase(contactIt);
			
			// Remove the Entity ID from the FoundIDs vector
			auto foundIt = find(mFoundIDs.begin(), mFoundIDs.end(), otherID);
			if (foundIt != mFoundIDs.end())
				mFoundIDs.erase(foundIt);

			// Remove the Entity ID from the corresponding vector in the FoundIDsType map
			std::vector<int>* pTypeIDList = &mFoundIDsTypeMap[otherType];
			foundIt = find(pTypeIDList->begin(), pTypeIDList->end(), otherID);
			if (foundIt != pTypeIDList->end())
				pTypeIDList->erase(foundIt);
		}
	}
}

std::vector<int> AreaSearch::GetFound()
{
	return mFoundIDs;
}

std::vector<int> AreaSearch::GetFound(std::string a_type)
{
	return mFoundIDsTypeMap[a_type];
}

void AreaSearch::SortContacts(std::vector<int>& a_contantIDs)
{
	if (mCompareFunctionName == "Closest") {
		int compareID = mOwnerID;
		Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID);
		if (ENTITY_MANAGER->GetEntity(pEntity->GetParentID()))
			compareID = pEntity->GetParentID();

		std::sort(a_contantIDs.begin(), a_contantIDs.end(), Closest(mOwnerID));
	}
	else {
		// Empty
	}
}

GEAR_API void AreaSearch::Clone(int objID)
{
	AreaSearch *rhs = static_cast<AreaSearch*>(COMPONENT_MANAGER->GetComponent<AreaSearch>(objID, "AreaSearch"));
	
	mCompareFunctionName = rhs->mCompareFunctionName;
	mSearchTypes = rhs->mSearchTypes;
}