#include "pch.h"
#include "Building.h"
#include "../Engine.h"
#include "../Entity.h"
#include "Utility.h"
#include "Core.h"
#include "Wall.h"
#include "Audio.h"
#include "LevelButton.h"
#include "Collider.h"
#include "Transform.h"
#include "ModelComponent.h"
#include "SpotLight.h"
#include "ImGui/imgui_stdlib.h"
#include "Resources/JsonData.h"
#include "Managers/ComponentManager.h"
#include "Managers/Log.h"
#include "Managers/EventManager.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/InputManager.h"
#include "Managers/CameraManager.h"
#include "Managers/ObjectFactory.h"
#include "Managers/EnemyStateManager.h"
#include "Managers/Renderer.h"
#include "Managers/SceneManager.h"
#include "Physics/PhysicsManager.h"
#include "Editor/LevelEditor.h"



Building::Building():Component("Building")
{
	mChild = nullptr;
	mWorldID = 0;
	once = false;
	mChildID = 0;
}

Building::~Building()
{
}

void Building::Init()
{
	std::vector<int> lightID = ENTITY_MANAGER->GetEntity(mOwnerID)->GetChildIDs("SpotLight");
	mChildID = lightID[0];
	mChild = COMPONENT_MANAGER->GetComponent<SpotLight>(mChildID, "SpotLight");
	once = true;
}

void Building::Reset()
{
	mChild = nullptr;
	mWorldID = 0;
	once = false;
	mChildID = 0;
	Component::Reset();
}

Building * Building::Create()
{
	return new Building();
}

void Building::Update(float a_deltaTime)
{
	if (once) {
		int temp = SCENE_MANAGER->GetCompletedWorld();
		if (mWorldID <= temp)
			ENTITY_MANAGER->EntityEnable(mChildID, true);
		else
			ENTITY_MANAGER->EntityEnable(mChildID, false);
		once = false;
	}
}

void Building::DisplayDebug()
{
	
}

void Building::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["WorldID"];
	if (!dataRoot.isNull())
		mWorldID = dataRoot.asInt();
}

void Building::DeSerialize(Json::Value & a_root, bool a_ignoreDefault)
{
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Building"];

	if (mWorldID != defaultRoot["WorldID"].asInt() || a_ignoreDefault)
		a_root["WorldID"] = mWorldID;
}

void Building::HandleEvent(Event * a_pEvent)
{
	
}

void Building::Clone(int objID)
{
	
}
