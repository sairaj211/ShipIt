#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "RayCast.h"
#include "../Engine.h"
#include "../Managers/CameraManager.h"
#include "../Components/Transform.h"
#include "../Components/ModelComponent.h"
#include "../Managers/EntityManager.h"
#include "../Managers/ComponentManager.h"
#include "Physics/PhysicsManager.h"
#include "Managers/FrameRateManager.h"
#include "../Entity.h"
#include "../Components/Bullet.h"


#pragma comment(lib, "glu32.lib") 
#include <gl/GLU.h>

RayCast* RayCast::mInstance = nullptr;

RayCast* RayCast::GetInstance()
{
	if (!mInstance) {
		mInstance = new RayCast();
	}
	return mInstance;
}

void RayCast::Init()
{
	LOG_INFO("RayCaster Init");
	mRay = new Ray();
	mCrossHair = nullptr;


}

void RayCast::CleanUp()
{
	delete mInstance;
}

void RayCast::MoveCrosshair(float x1, float y1) {

	/*
	float x = (2.0f * x1) / Engine::mWindowWidth - 1.0f;
	float y = 1.0f - (2.0f * y1) / Engine::mWindowHieght;
	glm::vec3 ray_nds = glm::vec3(x, y, 1);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);//(no need of to reverse perspective division,   ray has not   depth) 
	glm::vec4 ray_eye = glm::inverse(BASIC_CAMERA->GetProjection())   *    ray_clip;
	ray_eye = glm::vec4(glm::vec2(ray_eye), -1.0, 0.0);
	glm::vec3 direction = glm::vec3(glm::inverse(BASIC_CAMERA->GetViewMatrix()) *  ray_eye);
	direction = glm::normalize(glm::vec3(direction.x, direction.y, direction.z));

	if (!mCrossHair) {
		std::vector<Component*> trColliderList = COMPONENT_MANAGER->GetComponents("Collider");//obtain list of colliders
		for (Component* pComponent : trColliderList) {
			Collider * myCollider = static_cast<Collider*>(pComponent);//obtain collider
			if (myCollider->mLType == "CROSSHAIR") {
				mCrossHair = myCollider;
				break;
			}
		}
	}
	if (mCrossHair) {
		Transform * chTransform = COMPONENT_MANAGER->GetComponent<Transform>(mCrossHair->GetOwnerID(), "Transform");
		glm::vec3 tempPos = chTransform->GetPosition();
		glm::vec3 tempRot = chTransform->GetRotationAngles();
		tempPos = BASIC_CAMERA->GetPosition() + direction * 0.8f;;;;;;;;;;;;//  result.second;
		tempRot = glm::vec3(0.0f, 90.0f, 0.0f);
		chTransform->SetPosition(tempPos);
		chTransform->SetRotationAngles(tempRot);
	}
	*/
}

int RayCast::CastRay(float x1, float y1, bool isBullet)
{

	//0.-SCREEN POSITION  (in pixels) screen space
	//  WINDOW -> NDC  (Clip space)
	float w, h;
	Engine::GetWindowSize(w, h);
	float x = (2.0f * x1) / w - 1.0f;
	float y = 1.0f - (2.0f * y1) / h;
	glm::vec3 ray_nds = glm::vec3(x, y, 1);
// NDC -> CLIP
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, -1.0, 1.0);//(no need of to reverse perspective division,   ray has not   depth) 

	glm::vec4 ray_eye = glm::inverse(CAMERA_MANAGER->GetCameraProjectionMatrix())   *    ray_clip;

	ray_eye = glm::vec4(glm::vec2(ray_eye), -1.0, 0.0);
	glm::vec3 direction = glm::vec3(glm::inverse(CAMERA_MANAGER->GetCameraViewMatrix()) *  ray_eye);
	
	direction = glm::normalize(glm::vec3(direction.x, direction.y, direction.z));
//	std::cout << "Direction = " << direction.x << " " << direction.y << " " << direction.z << " " << std::endl;
	//std::cout << "RayDirection = " << direction.x << " " << direction.y << " " << direction.z << std::endl << std::endl;
	mRay->origin = CAMERA_MANAGER->GetCameraPosition();
	//std::cout << "mRay->origin = " << mRay->origin.x << " " << mRay->origin.y << " " << mRay->origin.z << " " << std::endl;
	mRay->direction = direction;
	
	std::pair<Collider *, glm::vec3> result = PHYSICS_MANAGER->Raycast(mRay);
	if (result.first) { // the object is Found 

		return result.first->GetOwnerID();

		int id = result.first->GetOwnerID();
		Entity* temp = ENTITY_MANAGER->GetEntity(id, true);

			if(isBullet)
				playerEntity = ENTITY_MANAGER->CreateRuntimeEntity("Bullet");
			else if(!isBullet)
				playerEntity = ENTITY_MANAGER->CreateRuntimeEntity("Grenade");


			objTrans = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(playerEntity->GetID(), "Transform");
			objBullet = COMPONENT_MANAGER->GetRuntimeComponent<Bullet>(playerEntity->GetID(), "BulletComp");
			objBody = COMPONENT_MANAGER->GetRuntimeComponent<Body>(playerEntity->GetID(), "Body");

			objBullet->owner = "Player";
			objBullet->type = playerEntity->GetDataFile();

			std::vector<Component*> components = COMPONENT_MANAGER->GetRuntimeComponents(playerEntity->GetID());

			for (Component* compo : components)
			{
				compo->Init();
			}

			std::vector<Entity*> Playerobject = ENTITY_MANAGER->GetEntities("PlayerObject");
			for (auto p : Playerobject)
			{
				playerTrans = COMPONENT_MANAGER->GetComponent<Transform>(p->GetID(), "Transform");
			}

			if (isBullet) {
				ShootBullet(result);
			}
			else if (!isBullet) {
				Grenade(result);
			}
	
		//}
	}
	return -1;


		//http://antongerdelan.net/opengl/raycasting.html
		//https://people.cs.clemson.edu/~dhouse/courses/405/notes/raycast.pdf
}

void RayCast::ShootBullet(std::pair<Collider*, glm::vec3> result)
{
	 
	//bulletO = mRay->origin;

	objBullet->mStartPos = playerTrans->GetPosition();
	objBullet->mStartPos.y += 1.0f;

	mRay->origin = objBullet->mStartPos;

	//mBulletComp->mEndPos = mRay->origin + mRay->direction * 100.0f;

	objBullet->mEndPos = result.second;

	glm::vec3 tempPos = objTrans->GetPosition();
	tempPos = objBullet->mStartPos;
	objBullet->mDirection = glm::normalize(result.second - objBullet->mStartPos);
	objTrans->SetPosition(tempPos);
}

void RayCast::Grenade(std::pair<Collider*, glm::vec3> result)
{
	objBullet->mStartPos = playerTrans->GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 origin = objBullet->mStartPos;
	glm::vec3 destiny = result.second;


	glm::vec3 tempBulletPos = objTrans->GetPosition();
	tempBulletPos = objBullet->mStartPos;
	objTrans->SetPosition(tempBulletPos);

	float distanceReference = abs(destiny.x - origin.x) + abs(destiny.y - origin.y) + abs(destiny.z - origin.z);
	float maxrefDistance = 12.0f;
	float minrefDistance = 5.0f;
	float maxTimeCollide = 1.5;
	float minTimeCollide = 0.5f;

	float timetoCollide = distanceReference > maxrefDistance ? maxTimeCollide :
		distanceReference < minrefDistance ? minTimeCollide :
		(maxTimeCollide - minTimeCollide) * (1 - (maxrefDistance - distanceReference) / maxrefDistance) + minTimeCollide;

	glm::vec3 VELOCITY = glm::vec3(0, 0, 0);
	glm::vec3 tempPos = playerTrans->GetPosition();
	VELOCITY.x = (destiny.x - tempPos.x) / timetoCollide;
	VELOCITY.y = ((destiny.y - tempPos.y) + (9.81f * timetoCollide*timetoCollide * 0.5f)) / timetoCollide;
	VELOCITY.z = (destiny.z - tempPos.z) / timetoCollide;
	objBody->mVel = VELOCITY; 

}

void RayCast::IdentifyBullet()
{
	if (!mBulletCollider) {
		std::vector<Component*> trColliderList = COMPONENT_MANAGER->GetComponents("Collider");//obtain list of colliders
		for (Component* pComponent : trColliderList) {
			Collider * myCollider = static_cast<Collider*>(pComponent);//obtain collider
			if (myCollider->mLType == "BULLET") {
				mBulletCollider = myCollider;
				break;
			}
		}
		mBuletTransform = COMPONENT_MANAGER->GetComponent<Transform>(mBulletCollider->GetOwnerID(), "Transform");
	}
}


void RayCast::MoveBullet()
{
	//	IdentifyBullet();
	if (mBuletTransform != nullptr) {
		glm::vec3 tempPos = objTrans->GetPosition();
		tempPos += mBulletDir * float(FRAMERATE_MANAGER->GetFrameTime() / 1000.0f) *25.0f;
		mBuletTransform->SetPosition(tempPos);
	}
}
RayCast::RayCast()
{

}

RayCast::~RayCast()
{

}

