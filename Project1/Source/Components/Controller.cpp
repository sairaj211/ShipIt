#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Controller.h"
#include "Engine.h"
#include "Managers/InputManager.h"
#include "Resource.h"
#include "Entity.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Resource.h"
#include "Resources/JsonData.h"

Controller::Controller() : Component("Controller")
{


}

Controller::~Controller()
{

}

void Controller::Reset()
{
	mPlayerId = -1;
	Component::Reset();
}

Controller* Controller::Create()

{
	return new Controller();
}

void Controller::Update(float deltaTime)
{
	
}


void Controller::DisplayDebug()
{
	ImGui::InputInt("Controller #", &mPlayerId);
}

void Controller::HandleEvent(Event * pEvent)
{
}

bool Controller::IsPressed(std::string pressed)
{
	return INPUT_MANAGER->IsPressed(pressed,mPlayerId);
}

bool Controller::IsTriggered(std::string trigger)
{
	return INPUT_MANAGER->IsTriggered(trigger, mPlayerId);
}

bool Controller::IsReleased(std::string release)
{
	return INPUT_MANAGER->IsReleased(release, mPlayerId);
}

void Controller::SetId(int _playerId)
{
	mPlayerId = _playerId;
}

int Controller::GetControllerId()
{
	return mPlayerId;
}

GEAR_API void Controller::ConfirmController(bool aConfirm)
{
	INPUT_MANAGER->ConfirmGamepad(mPlayerId, aConfirm);
}

void Controller::Serialize(Json::Value root)
{
	Json::Value dataRoot = root["player"];
	if (!dataRoot.isNull()) {
		mPlayerId = dataRoot.asInt();
	}
}

void Controller::DeSerialize(Json::Value& root, bool a_ignoreDefault)
{
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Controller"];
	if (mPlayerId != defaultRoot["player"].asInt() || a_ignoreDefault)
		root["player"] = mPlayerId;
}
	

GEAR_API void Controller::Clone(int objID)
{
	Controller *rhs = static_cast<Controller*>(COMPONENT_MANAGER->GetComponent<Controller>(objID, "Controller"));

	mPlayerId = rhs->mPlayerId;
}

