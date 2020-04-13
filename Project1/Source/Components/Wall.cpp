#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Wall.h"
#include "Transform.h"
#include "ModelComponent.h"
#include "../Managers/ComponentManager.h"
#include "../Managers/EntityManager.h"
#include "Physics/PhysicsManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/EnemyStateManager.h"
#include "../Editor/RayCast.h"
#include "../Entity.h"
#include "Audio.h"
#include "../Engine.h"
#include "Collider.h"
#include "ParticlesComponent.h"
#include "Bullet.h"


Wall::Wall() : Component("Wall")
{
	mHealth = 300;
	mType = " ";
	mTotalKills = 0;
}


Wall::~Wall()
{
}



void Wall::Reset()
{

}

Wall* Wall::Create()
{
	return new Wall();
}

void Wall::Update(float a_deltaTime)
{
	mTotalKills = ENEMY_STATE_MANAGER->mEnemiesKilled; 
}

void Wall::DisplayDebug()
{

}

void Wall::Serialize(Json::Value a_root)
{
	Json::Value::Members componentNames = a_root.getMemberNames();
	Json::Value dataRoot = a_root["mType"];
	if (!dataRoot.isNull())
		mType = dataRoot.asString();
}

void Wall::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{

}

void Wall::Destroy()
{
	ENTITY_MANAGER->DeleteEntity(mOwnerID);

}

void Wall::Init()
{

	mwallModel = COMPONENT_MANAGER->GetComponent<ModelComponent>(mOwnerID, "ModelComponent");
	if (!mwallModel)
		mwallModel = COMPONENT_MANAGER->GetRuntimeComponent<ModelComponent>(mOwnerID, "ModelComponent");


	mWallCollider = COMPONENT_MANAGER->GetComponent<Collider>(mOwnerID, "Collider");
	if (!mWallCollider)
		mWallCollider = COMPONENT_MANAGER->GetRuntimeComponent<Collider>(mOwnerID, "Collider");


	mWallTransform = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
	if (!mWallTransform)
		mWallTransform = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");

	mPos = mWallTransform->GetPosition();
}

void Wall::HandleEvent(Event* a_pEvent)
{

}


void Wall::ChangeColor(std::string color)
{
	mwallModel->mpMaterial = RESOURCE_MANAGER->LoadMaterial(color);
}
