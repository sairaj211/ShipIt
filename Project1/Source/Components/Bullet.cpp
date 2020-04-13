#include "pch.h"
#include "Managers/Log.h"

#include "Core.h"
#include "Bullet.h"
#include "Transform.h"
#include "ModelComponent.h"
#include "../Managers/ComponentManager.h"
#include "../Managers/EntityManager.h"
#include "../Physics/PhysicsManager.h"
#include "../Editor/RayCast.h"
#include "../Entity.h"
#include "Audio.h"
#include "../Engine.h"
#include "Collider.h"
#include "ParticlesComponent.h"

Bullet::Bullet() : Component("BulletComp")
{
	mStartTimer = 0.0f;
	mMaxTime = 5.0f;
	mStartPos = glm::vec3(0.0f);
	mEndPos = glm::vec3(0.0f);
	mVelocity = 20.0f;
	mRay = new Ray();
	mGTimer = 0.0f;
	mPlayOnce = false;
}

Bullet::~Bullet()
{
}

void Bullet::Reset()
{
	mStartTimer = 0.0f;
	mMaxTime = 2.0f;
	mStartPos = glm::vec3(0.0f);
	mEndPos = glm::vec3(0.0f);
	mVelocity = 50.0f;
	Component::Reset();
}

Bullet* Bullet::Create()
{
	return new Bullet();
}

void Bullet::Update(float a_deltaTime)
{
	//BULLET MOVEMENT 
	mStartTimer += a_deltaTime / 1000.0f;

	//	LOG_INFO(mStartTimer);
	if (mStartTimer <= mMaxTime) 
	{
		glm::vec3 tempBulletPos = mBulletTrans->GetPosition();

		//ENEMY ONLY
		if (owner == "Enemy")
			tempBulletPos += mDirection * (a_deltaTime / 1000.0f) * mVelocity;


		//PLAYER ONLY
		if (owner == "Player") {
			if(type =="Bullet")
			{
				tempBulletPos = mEndPos;
				mModel->mIsRenderable = false;

				//if (mParticle && !mPlayOnce) {
				//	//mParticle->mPartPos = mEndPos;
				//	//->SetRunning(true);

				////	glm::vec3 playerT = playerTrans->GetPosition();
				////	playerParticle->mPartPos.x = playerT.x + 0.25f;
				////	playerParticle->mPartPos.y = playerT.y + 1.25f;

				////	playerParticle->SetRunning(true);
				//}
				if (mStartTimer >= 0.2f && !mPlayOnce)
				{
					//mParticle->SetRunning(false);
				//	playerParticle->SetRunning(false);
					mPlayOnce = true;
					Destroy();
				}

			}
		}

		if (mCollid->mIsCollided)
		{
			if (type == "Grenade")
			{
				mGTimer += a_deltaTime / 1000.0f;
				mCollid->mRescale = glm::vec3(5.0f);
				if (!mPlayOnce)
				{
					glm::vec3 trans = mBulletTrans->GetPosition();
					mAudio->Play();
					mAudio->Volume(20.0f);
					//mGrenadeParticle->mPartPos = trans;
					//mGrenadeParticle->SetRunning(true);
					
					
					mPlayOnce = true;
				}
			}
			if (mGTimer > 3.0f) {
				Destroy();
				mGTimer = 0.0f;
			}

		}
		mBulletTrans->SetPosition(tempBulletPos);
	}

	else {
		Destroy();
	}
}

void Bullet::DisplayDebug()
{

}

void Bullet::Serialize(Json::Value a_root)
{

}

void Bullet::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{

}

void Bullet::Destroy()
{
	/*if(owner == "Player")
		mParticle->SetRunning(false);*/
	ENTITY_MANAGER->DeleteEntity(mOwnerID);
	mAudio->Stop();
}

 void Bullet::Init()
{
	mBulletTrans = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
	if (!mBulletTrans)
		mBulletTrans = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");

	mCollid = COMPONENT_MANAGER->GetComponent<Collider>(mOwnerID, "Collider");
	if (!mCollid)
		mCollid = COMPONENT_MANAGER->GetRuntimeComponent<Collider>(mOwnerID, "Collider");

	mAudio = COMPONENT_MANAGER->GetComponent<Audio>(mOwnerID, "Audio");
	if (!mAudio)
		mAudio = COMPONENT_MANAGER->GetRuntimeComponent<Audio>(mOwnerID, "Audio");

	mModel = COMPONENT_MANAGER->GetComponent<ModelComponent>(mOwnerID, "ModelComponent");
	if (!mModel)
		mModel = COMPONENT_MANAGER->GetRuntimeComponent<ModelComponent>(mOwnerID, "ModelComponent");

	if (owner == "Player")
	{
		Entity* temp;
		for (std::pair<int, Entity*> element : ENTITY_MANAGER->mEntityMap) {
			temp = element.second;
			if (temp != nullptr) {
				std::string bc = temp->GetDataFile();
				if (bc == "Particle")
				{
					mParticle = COMPONENT_MANAGER->GetComponent<ParticlesComponent>(temp->GetID(), "ParticlesComponent");
					//break;
				}
			}

// 			if (temp != nullptr) {
// 				std::string bc = temp->GetDataFile();
// 				if (bc == "PlayerObject")
// 				{
// 					playerParticle = COMPONENT_MANAGER->GetComponent<ParticlesComponent>(temp->GetID(), "ParticlesComponent");
// 					playerTrans = COMPONENT_MANAGER->GetComponent<Transform>(temp->GetID(), "Transform");
// 					//break;
// 				}
// 			}

		}
		mAudio->m3D_sound = false;
	}

	if (type == "Grenade") {
		mGrenadeParticle = COMPONENT_MANAGER->GetComponent<ParticlesComponent>(mOwnerID, "ParticlesComponent");
		if (!mGrenadeParticle)
			mGrenadeParticle = COMPONENT_MANAGER->GetRuntimeComponent<ParticlesComponent>(mOwnerID, "ParticlesComponent");
	}
	if (type == "Bullet") {
		mAudio->Play();
		mAudio->Volume(0.4f);
	}

	if (type == "Grenade")
		mAudio->m3D_sound = true;
	

}

 void Bullet::HandleEvent(Event * a_pEvent)
{

}
