#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Player.h"

#include "Managers/ResourceManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/EventManager.h"
#include "Managers/ComponentManager.h"
#include "Physics/PhysicsManager.h"
#include "Managers/EnemyStateManager.h"
#include "Managers/EntityManager.h"
#include "Resources/JsonData.h"
#include "Managers/InputManager.h"
#include "Editor/RayCast.h"
#include "Bullet.h"
#include "Input/Mouse.h"
#include "Input/GamePad.h"
#include "Events/PauseEvent.h"
#include "Entity.h"
#include "Engine.h"

#include "Managers/CameraManager.h"
#include "Managers/AudioManager.h"
#include "Managers/SceneManager.h"

#include "../TestDefine.h"

void  Player::Update(float _deltaTime)
{

	AUDIO_MANAGER->UpdateListener3D(playerTransform->GetPosition(), glm::vec3(1.0f,0.0f,0.0f), glm::vec3(0.0f, 0.0f, -1.0f),glm::vec3(0.0f, 1.0f, 0.0f));


	currentDelta += _deltaTime / 1000.0f;

	if (!mAttack) {
		mRechargeTime -= currentDelta;
		if (mRechargeTime < 0) {
			mAttack = true;
		}

	}

#ifdef PROTOTYPE

	mKilled = ENEMY_STATE_MANAGER->mEnemiesKilled;


	glm::vec2 mousePos;
	mousePos = Mouse::GetInstance()->GetMouseScreenPos();
	RAYCAST->MoveCrosshair(mousePos.x, mousePos.y);
	if (Mouse::GetInstance()->IsPressed(LEFT) && !ImGui::IsAnyWindowHovered()) {
		if (currentDelta - PrevDelta > mFiringRate) {
			RAYCAST->CastRay(mousePos.x, mousePos.y, true);
			PrevDelta = currentDelta;
			// 			glm::vec3 rot = playerTransform->GetRotationAngles();
			// 			rot.y = 180.0f;
			// 			playerTransform->SetRotationAngles(rot);
		}
	}

	if (INPUT_MANAGER->IsReleased(SDL_SCANCODE_G) && !ImGui::IsAnyWindowHovered()) {
		RAYCAST->CastRay(mousePos.x, mousePos.y, false);
	}

#endif

	Collider* playerCollider = COMPONENT_MANAGER->GetComponent<Collider>(mOwnerID, "Collider");
	//Move/Jump
	ConstrainVelocityPlayer();
	CheckGroundedPlayer();
	
	if (!mPaused) {
		float yAngle = playerTransform->GetRotationAngles().y;
		glm::vec3 playerRot = playerTransform->GetRotationAngles();

		float curAngle = fmod(playerRot.y, 360.0f);
		curAngle = fmod(curAngle + 360.0f, 360.0f);
		float diffAngle = mAngle - curAngle;
		float turnAngle = 0;

		if (-180.f < diffAngle && diffAngle <= 180)
			turnAngle = diffAngle;
		else if (diffAngle > 180)
			turnAngle = diffAngle - 360.f;
		else
			turnAngle = diffAngle + 360.f;

		playerRot.y += turnAngle * _deltaTime*.01f;
		playerTransform->SetRotationAngles(playerRot);
	}

}
Player::Player(): Component("Player")
{
	mInitialized = false;
	
}

Player::~Player()
{
}


Player * Player::Create()
{
	return new Player();
}

void Player::Init() {
	if (!mInitialized) {
		mAttack = false;
		mHitTime = 0;
		mRechargeTime = 0.f;
		//GET REFERENCE TO THE PLAYERS BODY
		playerTransform = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform", true);
		if (!playerTransform)
			playerTransform = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");
		mAngle = playerTransform->GetRotationAngles().y;

		
		mInitialized = true;
		mFiringRate = 0.2f;
		currentDelta = PrevDelta = 0.0f;
		mKilled = 0;
	}

}

void Player::ConstrainVelocityPlayer()
{

}

void Player::SetMaxVelocity(float aMax)
{
	mMaxVelocity = aMax;
}

void  Player::CheckGroundedPlayer()
{
	std::vector<int> invisibleCubes = ENTITY_MANAGER->GetEntity(mOwnerID)->GetChildIDs("InvisibleCube");
	if (invisibleCubes.size() > 0) {
		Collider* pCollider = COMPONENT_MANAGER->GetComponent<Collider>(invisibleCubes[0], "Collider");
		gnd = PHYSICS_MANAGER->OnTriggerEnter(pCollider) || PHYSICS_MANAGER->OnTriggerStay(pCollider);
	}
}

bool Player::GetIsGrounded() 
{
	return gnd;
}

void Player::Reset()
{
	playerTransform = nullptr;
	bOffset = velPlayer = glm::vec3(0.0f);
	gnd = true;
	mMaxVelocity = 4.0f;

	mInitialized = false;
	Component::Reset();
}

void Player::MakeAttack()
{

	
}

void Player::HandleEvent(Event * a_pEvent)
{
	if (a_pEvent->GetType() == "PauseEvent") {
		mPaused = static_cast<PauseEvent*>(a_pEvent)->mPause;
	}

	if (a_pEvent->GetType() == "LevelCompleteEvent")
	{
		LevelCompleteEvent* pLCEvent = static_cast<LevelCompleteEvent*>(a_pEvent);
		std::string temp = SCENE_MANAGER->GetCurrent();
		int length = temp.length();
		std::string CompletedLevel = temp.substr(0, length - 5);

		std::vector<LevelData> *levelData = SCENE_MANAGER->GetLevelData();

		for (LevelData &element : *levelData)
		{

			if (element.LevelName.compare(CompletedLevel) == 0)
			{
				if (element.highScore < pLCEvent->mScore) {
					element.highScore = pLCEvent->mScore;
					element.grade = pLCEvent->mGrade[0];
				}
			}
		}

	}
}

void Player::DisplayDebug() {}

void Player::Serialize(Json::Value _root)
{
	Json::Value dataRoot = _root["mRecharge"];
	if (!dataRoot.isNull()) 
		mRecharge = _root["mRecharge"].asFloat();

	dataRoot = _root["mHitRecharge"];
	if (!dataRoot.isNull())
		mHitRecharge = _root["mHitRecharge"].asFloat();
	
}

void Player::DeSerialize(Json::Value& _root, bool a_ignoreDefault)
{
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;

	if (defaultRoot["mRecharge"] != mRecharge || a_ignoreDefault)
		_root["mRecharge"] = mRecharge;

	if (defaultRoot["mHitRecharge"] != mHitRecharge || a_ignoreDefault)
		_root["mHitRecharge"] = mHitRecharge;
}

void Player::Move(float aX, float aY, bool sprint)
{
	Body* playerBody = COMPONENT_MANAGER->GetComponent<Body>(mOwnerID, "Body");
	velPlayer = glm::vec3(aX, 0, aY);
	if (isinf(aX) || isinf(aY)) return;
	float MaxVel = mMaxVelocity;
	if (!mAttack && mRechargeTime > mRecharge / 2) {
		MaxVel *= 2.f;
	}
	if (mAttack && sprint) {
		mAttack = false;
		//velPlayer *= 2.f;
		mRechargeTime = mRecharge;
		playerBody->mVel = velPlayer;
	}
	else {
		glm::vec3  vplyr = playerBody->mVel;
		if (vplyr.x == 0) {
			playerBody->mVel.x = velPlayer.x;
		}

		else if(vplyr.x > 0 && velPlayer.x > 0) {
			if (vplyr.x <= velPlayer.x) 

				playerBody->mVel.x = velPlayer.x > MaxVel ? MaxVel : velPlayer.x;
		}
		else  if (vplyr.x < 0 && velPlayer.x < 0) {
			if (vplyr.x > velPlayer.x)
				playerBody->mVel.x = velPlayer.x < -MaxVel ? -MaxVel : velPlayer.x;
		}
		else {
			playerBody->mVel.x = velPlayer.x;
		}
		if (vplyr.z == 0) {
			playerBody->mVel.z = velPlayer.z;
		}
		else if (vplyr.z > 0 && velPlayer.z > 0) {
			if (vplyr.z < velPlayer.z)
				playerBody->mVel.z = velPlayer.z > MaxVel ? MaxVel : velPlayer.z;
		}
		else  if (vplyr.z < 0 && velPlayer.z < 0) {
			if (vplyr.y > velPlayer.y)
				playerBody->mVel.z = velPlayer.z < -MaxVel ? -MaxVel : velPlayer.z;
		}
		else {
			playerBody->mVel.z = velPlayer.z;
		}
	}

}	

void  Player::Jump(float aJumpForce)
{
	Body* playerBody = COMPONENT_MANAGER->GetComponent<Body>(mOwnerID, "Body");
	if (gnd) {
		gnd = false;
		playerBody->mVel.y =aJumpForce;
	}
}

void Player::Rotate(float _angleY)
{
	if (mAngle > 180 && _angleY == 0)
		mAngle = 360.f;
	else
		mAngle = _angleY;
}

void Player::Clone(int objID)
{

	Player *rhs = static_cast<Player*>(COMPONENT_MANAGER->GetComponent<Player>(objID, "Player"));

	mRecharge = rhs->mRecharge;
	mHitRecharge = rhs->mHitRecharge;

}
