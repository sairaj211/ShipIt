#include "pch.h"
#include "Teleport.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Wall.h"
#include "Transform.h"
#include "ModelComponent.h"
#include "../Managers/ComponentManager.h"
#include "../Managers/EntityManager.h"
#include "Physics/PhysicsManager.h"
#include "Managers/ResourceManager.h"
#include "Editor/LevelEditor.h"
#include "Resources/JsonData.h"
#include "Managers/EnemyStateManager.h"
#include "../Entity.h"
#include "Audio.h"
#include "../Engine.h"
#include "Collider.h"
#include "Events/ContactEvent.h"
#include "ImGui/imgui_stdlib.h"


Teleport::Teleport() : Component("Teleport")
{
	mPairID = 0;
	mUpdatePairs = true;
	mOtherObjectID = 0;
	mDirection = "minus";
}

Teleport::~Teleport()
{
}

void Teleport::Init()
{
	
}

void Teleport::Reset()
{
	mPairID = 0;
	mUpdatePairs = true;
	mOtherObjectID = 0;
	mDirection = "minus";

	Component::Reset();
}

Teleport * Teleport::Create()
{
	return new Teleport();
}

void Teleport::Update(float a_deltaTime)
{
	//std::cout << int(mPair[0]) << std::endl;
	if (mUpdatePairs)
	{
		std::vector<Component*> mTeleList = COMPONENT_MANAGER->GetComponents("Teleport");
		//LOG_INFO("YES");
		for (Component *element : mTeleList) {
			Teleport *pTeleport = static_cast<Teleport*>(element);

			if (pTeleport->mOwnerID != this->mOwnerID && pTeleport->mPairID == this->mPairID)
			{
				mOtherObjectID = pTeleport->mOwnerID;
				//LOG_INFO(mOtherObjectID);
			}
		}
		mUpdatePairs = false;
	}
}

void Teleport::DisplayDebug()
{
	ImGui::InputInt("Pair ID", &mPairID);
	ImGui::InputText("Direction", &mDirection);

	int LastID = LEVEL_EDITOR->GetLastPairID();
	if (mPairID > LastID) {
		LEVEL_EDITOR->SetLastPairID(mPairID);
	}

	std::vector<Component*> mTeleList = COMPONENT_MANAGER->GetComponents("Teleport");
	for (Component *element : mTeleList) {
		Teleport *pTeleport = static_cast<Teleport*>(element);
		pTeleport->mUpdatePairs = true;
		pTeleport->mOtherObjectID = 0;
	}
}

void Teleport::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["PairID"];
	if (!dataRoot.isNull()) {
		mPairID = dataRoot.asInt();

		int LastID = LEVEL_EDITOR->GetLastPairID();
		if (mPairID > LastID) {
			LEVEL_EDITOR->SetLastPairID(mPairID);
		}
	}

	dataRoot = a_root["Direction"];
	if (!dataRoot.isNull())
		mDirection = dataRoot.asString();

}

void Teleport::DeSerialize(Json::Value & a_root, bool a_ignoreDefault)
{
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Teleport"];

	if (mPairID != defaultRoot["PairID"].asInt() || a_ignoreDefault)
		a_root["PairID"] = mPairID;

	if (mDirection != defaultRoot["Direction"].asString() || a_ignoreDefault)
		a_root["Direction"] = mDirection;
}

void Teleport::HandleEvent(Event * a_pEvent)
{
	if (a_pEvent->GetType() == "ContactEvent") {
		ContactEvent* pContactEvent = static_cast<ContactEvent*>(a_pEvent);
		//LOG_INFO("YES");

		int otherID = -1;
		if (mOwnerID == pContactEvent->mId1)
			otherID = pContactEvent->mId2;
		if (mOwnerID == pContactEvent->mId2)
			otherID = pContactEvent->mId1;
		if (otherID == -1)
			return;

		if (mOtherObjectID != 0 &&  pContactEvent->mContactType == OnTriggerEnter) {
			Transform* mPlayer = static_cast<Transform*>(COMPONENT_MANAGER->GetComponent<Transform>(otherID, "Transform", true));
			
			Transform* mPortal = static_cast<Transform*>(COMPONENT_MANAGER->GetComponent<Transform>(mOtherObjectID, "Transform", true));

			glm::vec3 mPlayerTrans = mPlayer->GetLocalPosition();
			glm::vec3 mPortalTrans = mPortal->GetLocalPosition();

			float dir;
			if (mDirection == "plus")
				dir = 10.0f;
			else if (mDirection == "minus")
				dir = -10.0f;

			mPlayerTrans.x = mPortalTrans.x;
			mPlayerTrans.y = mPortalTrans.y;
			mPlayerTrans.z = mPortalTrans.z + dir;

			mPlayer->SetLocalPosition(mPlayerTrans);
			
		}

	}
}

void Teleport::Clone(int objID)
{
	
}
