#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "GridWaypoint.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/EventManager.h"
#include "Resources/JsonData.h"
#include "Entity.h"

GridWaypoint::GridWaypoint() : Component("GridWaypoint")
{
	mMaxDuration = mMinDuration = 90;
	mOrder = -1;
}

GridWaypoint::~GridWaypoint()
{

}

void GridWaypoint::Reset()
{
	mMaxDuration = mMinDuration = 90;
	mOrder = -1;

	Component::Reset();
}

GridWaypoint* GridWaypoint::Create()
{
	return new GridWaypoint();
}

void GridWaypoint::Update(float a_deltaTime)
{

}

void GridWaypoint::DisplayDebug()
{
	ImGui::InputInt("Duration Min", &mMinDuration); // Valid for positive non-zero values less than or equal to mMaxDuration
	ImGui::InputInt("Duration Max", &mMaxDuration); // Valid for positive non-zero values greater than or equal to mMinDuration
	ImGui::InputInt("Order", &mOrder); // Valid for values -1 and above
}

void GridWaypoint::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["MaxDuration"];
	if (!dataRoot.isNull())
		mMaxDuration = dataRoot.asInt();

	dataRoot = a_root["MinDuration"];
	if (!dataRoot.isNull())
		mMinDuration = dataRoot.asInt();

	dataRoot = a_root["Order"];
	if (!dataRoot.isNull())
		mOrder = dataRoot.asInt();
}

void GridWaypoint::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["GridWaypoint"];

	if (mMaxDuration != defaultRoot["MaxDuration"].asInt() || a_ignoreDefault)
		a_root["MaxDuration"] = mMaxDuration;

	if (mMinDuration != defaultRoot["MinDuration"].asInt() || a_ignoreDefault)
		a_root["MinDuration"] = mMinDuration;

	if (mOrder != defaultRoot["Order"].asInt() || a_ignoreDefault)
		a_root["Order"] = mOrder;
}

void GridWaypoint::HandleEvent(Event* a_pEvent)
{

}

void GridWaypoint::Clone(int a_otherID)
{

}

int GridWaypoint::GetMaxDuration()
{
	return mMaxDuration;
}

int GridWaypoint::GetMinDuration()
{
	return mMinDuration;
}

int GridWaypoint::GetOrder()
{
	return mOrder;
}