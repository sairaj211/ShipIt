#include "pch.h"
#include "Managers/Log.h"

#include "Core.h"
#include "ButtonComponent.h"
#include "Collider.h"
#include "Transform.h"
#include "Player.h"
#include "Managers/ComponentManager.h"
#include "Physics/PhysicsManager.h"
#include "Engine.h"
ButtonComponent::ButtonComponent() : Component("ButtonComponent")
{
	mIMGUIHOLD = false;
}


ButtonComponent::~ButtonComponent()
{
}

GEAR_API ButtonComponent * ButtonComponent::Create()
{
	return new ButtonComponent();
}

GEAR_API void ButtonComponent::Update(float)
{
	//check for collisions here
	
	mIsPressed =
		PHYSICS_MANAGER->GetCollisionManager()->OnTriggerStay(mpMyCollider) || mIMGUIHOLD;
	mIsTriggered =
		PHYSICS_MANAGER->GetCollisionManager()->OnTriggerEnter(mpMyCollider);
	mIsReleased =
		PHYSICS_MANAGER->GetCollisionManager()->OnTriggerExit(mpMyCollider);

// 	if (IsItemHovered())
// 	{
// 
// 	}
}

GEAR_API void ButtonComponent::DisplayDebug()
{
	ImGui::Checkbox("Pressed", &mIsPressed);
	ImGui::Checkbox("Hold", &mIMGUIHOLD);
}

GEAR_API void ButtonComponent::Init()
{
	std::vector<Component*> playerList = COMPONENT_MANAGER->GetComponents("Player");//assume 1
	mPlayerId = -1;
	if (playerList.size() != 0) {
		mPlayerId = playerList[0]->GetOwnerID();
	}
	if (mPlayerId > 0) {//entity ids start at 1
		mpPlayerCollider = COMPONENT_MANAGER->GetComponent<Collider>(mPlayerId, "Collider");
		mpPlayerTransform = COMPONENT_MANAGER->GetComponent<Transform>(mPlayerId, "Transform");
	}
	mpMyCollider = COMPONENT_MANAGER->GetComponent<Collider>(mOwnerID, "Collider");
	if (!mpMyCollider)
		mpMyCollider = COMPONENT_MANAGER->GetRuntimeComponent<Collider>(mOwnerID, "Collider");
	mpMyTransform = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
	if (!mpMyTransform)
		mpMyTransform = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");
}

GEAR_API void ButtonComponent::SelfReset()
{
	mpMyTransform = mpPlayerTransform = nullptr;
	mpMyCollider = mpPlayerCollider = nullptr;
	mIsPressed = false;
	mIMGUIHOLD = false;
}

GEAR_API void ButtonComponent::Serialize(Json::Value)
{
	
}

GEAR_API void ButtonComponent::DeSerialize(Json::Value &a_root, bool a_ignoreDefault)
{
	
}

GEAR_API bool ButtonComponent::IsPressed()
{
	return mIsPressed;
}

GEAR_API bool ButtonComponent::IsTriggered()
{
	return mIsTriggered;
}

GEAR_API bool ButtonComponent::IsReleased()
{
	return mIsReleased;
}

GEAR_API void ButtonComponent::HandleEvent(Event * a_pEvent)
{
	
}

GEAR_API void ButtonComponent::Reset()
{
	mIsTriggered = mIsReleased = mIsPressed = false;
	mPlayerId = 0;
	mIMGUIHOLD = false;
	mpPlayerCollider = nullptr;
	mpMyCollider = nullptr;
	mpPlayerTransform = nullptr;
	mpMyTransform = nullptr;
	Component::Reset();

}

GEAR_API void ButtonComponent::Clone(int objID)
{
	ButtonComponent *rhs = static_cast<ButtonComponent*>(COMPONENT_MANAGER->GetComponent<ButtonComponent>(objID, "ButtonComponent"));

	mIsPressed = rhs->mIsPressed;
	mIsTriggered = rhs->mIsTriggered;
	mIsReleased = rhs->mIsReleased;
	mPlayerId = rhs->mPlayerId;
	mIMGUIHOLD = rhs->mIMGUIHOLD;
}
