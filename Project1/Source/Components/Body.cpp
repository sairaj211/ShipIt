/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
Author: Ivan Cazares, ivan.c, 60004418
- End Header --------------------------------------------------------*/
#include "pch.h"
#include "Managers/Log.h"

#include "Core.h"
#include "Body.h"
#include "Events/PauseEvent.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
#include "Entity.h"
#include "Event.h"
#include "Collider.h"
#define COMPONENT_MANAGER ComponentManager::GetInstance()

Body::Body() : Component("Body"), mUseGravity(true), mConstrainX(false), mConstrainY(false), mConstrainZ(false),
mPos(0.0f), mPrevPos(0.0f), mVel(0.0f), mAcc(0.0f), mMass(1), mApplyFriction(true), mOmega(0.0f), mAlpha(0.0f),
mPrevTheta(0.0f), mTheta(0.0f)
{
	mInvMass = mMass != 0.0f ? 1.0f / mMass : 0.0f;
	_transform = nullptr;
	mAllowRotations = false;
	mInvI_Body = glm::mat3(0);
	mI_Body = glm::mat3(0);
}

Body::~Body()
{

}

Body* Body::Create()
{
	return new Body();
}

void Body::Update(float a_deltaTime)
{
	if (isnan(mVel.x) || isinf(mVel.x) || isnan(mVel.y) || isinf(mVel.y) || isnan(mVel.z) || isinf(mVel.z))
		std::cout << "Issue iwth vel" << std::endl;
}

void  Body::DisplayDebug()
{
	ImGui::Checkbox("Use Gravity", &mUseGravity);
	ImGui::DragFloat("Mass", &mMass, 0.35f);
	ImGui::DragFloat("Inverse Mass", &mInvMass, 0.35f);


	ImGui::DragFloat3("Postion", &mPos[0], 0.35f);
	ImGui::DragFloat3("Previous Postion", &mPrevPos[0], 0.35f);
	ImGui::DragFloat3("Linear Force", &mLinearForce[0], 0.35f);
	ImGui::DragFloat3("Velocity", &mVel[0], 0.35f);
	ImGui::DragFloat3("Acceleration", &mAcc[0], 0.3f);

	ImGui::DragFloat3("Theta", &mTheta[0], 0.35f);
	ImGui::DragFloat3("Previous Theta", &mPrevTheta[0], 0.35f);
	ImGui::DragFloat3("Angular Velocity", &mOmega[0], 0.35f);
	ImGui::DragFloat3("Angular Acceleration", &mAlpha[0], 0.3f);

	ImGui::DragFloat3("InertialTenser1", &mI_Body[0][0], 0.35f);
	ImGui::DragFloat3("InertialTenser2", &mI_Body[1][0], 0.35f);
	ImGui::DragFloat3("InertialTenser3", &mI_Body[2][0], 0.35f);

	ImGui::Checkbox("ConstrainX", &mConstrainX);
	ImGui::Checkbox("ConstrainY", &mConstrainY);
	ImGui::Checkbox("ConstrainZ", &mConstrainZ);

}

void Body::Move(float a_rate)
{
	

}

void Body::ConstrainVelocity()
{
	mVel = glm::vec3(
		mConstrainX ? 0.0f : mVel.x,
		mConstrainY ? 0.0f : mVel.y,
		mConstrainZ ? 0.0f : mVel.z
	);
}

void Body::Init()
{
	mInvMass = mMass != 0.0f ? 1.0f / mMass : 0.0f;
	_transform = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform", true);
	if (!_transform)
		_transform = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");

	mPos = mPrevPos = _transform->GetPosition();
	mLinearForce = glm::vec3(0);
	mTorque = glm::vec3(0);
	mVel = glm::vec3(0);
	mAcc = glm::vec3(0);
}


void Body::SetConstantVelocity(glm::vec3 a_vel)
{
	/*
	if (!mIsConstantVelocitySet) {
		mIsConstantVelocitySet = true;
		mConstantVelocity = a_vel;
		mVel = a_vel;
	}*/
}

void Body::SelfReset()
{
	mUseGravity = false;
	mPos = mPrevPos = mLinearForce = mAcc = mVel = glm::vec3(0);
}

void Body::UpdateGravityForces(float a_fixedDeltaTime, glm::vec3 a_gravity)
{
	if (mPaused)
		return;

	if (mInvMass > 0)
	{
		glm::vec3 bodyGravity = mUseGravity ? a_gravity : glm::vec3(0, 0, 0);
		mAcc =  bodyGravity;//ACCELERATION
		mLinearForce = mLinearForce+ (mAcc* mInvMass);//VELOCITY
	}
}
GEAR_API void Body::UpdatePositions(float a_fixedDeltaTime)
{
	if (mPaused)
		return;

	mPos.x += !mConstrainX ? mVel.x*a_fixedDeltaTime : 0;
	mPos.y += !mConstrainY ? mVel.y*a_fixedDeltaTime : 0;
	mPos.z += !mConstrainZ ? mVel.z *a_fixedDeltaTime : 0;
	_transform->SetPosition(mPos);

	if (mAllowRotations) {
		glm::vec3 axis = glm::normalize(mOmega);
		float angle = glm::length(axis)*a_fixedDeltaTime;
		glm::mat3 rot = glm::rotate(angle, axis);
		mTheta = mTheta * rot;
		_transform->SetRotationAngles(mTheta);
	}
}
GEAR_API void Body::AddVelocities(glm::vec3 vel, glm::vec3 p, bool useMass)
{

	//std::cout << "Velocity was (" << mVel.x <<","<<mVel.y<<","<<mVel.z <<")" << std::endl;
	glm::vec3 F = vel;
	if (useMass)
		F *= mInvMass;
	mVel = mVel + F;//not sure about this
	//std::cout << "Velocity is now (" << mVel.x << "," << mVel.y << "," << mVel.z << ")" << std::endl;
	if (mAllowRotations) {
		glm::vec3 r = mPos - p;
		mOmega += mInvI_Body*(glm::cross(r, vel));
	}
}
GEAR_API void Body::ApplyForce(glm::vec3 F, glm::vec3 p)
{
	mLinearForce = mLinearForce + F*mInvMass;//linear force application
	if(mAllowRotations)
		mTorque = mTorque+ glm::cross(p - mPos,F);
}


void Body::Integrate(float a_fixedDeltaTime)
{
	if (mPaused)
		return;

	ConstrainVelocity(); // TMP fix
	mPos = _transform->GetPosition();
	mPrevPos = mPos;

	mTheta = _transform->GetRotationAngles();
	mPrevTheta = mTheta;
	//EULERS METHOD Linear velocity

	mVel += (mLinearForce * mInvMass*a_fixedDeltaTime);
	//if (mMass > 0 && GetOwnerID() == 40)
		//std::cout << "Current mVel (" << mVel.x << "," << mVel.y << "," << mVel.z << ")" << std::endl;
	
	mLinearForce = glm::vec3(0);

	//note if

	//Angular integration
	if (mAllowRotations) {//integrate the angular momentum if the shape should
		mAlpha = mTorque;
		mOmega += GetGlobalInverseTenser()*(mAlpha *a_fixedDeltaTime);
		
	}
	mTorque = glm::vec3(0);
}

void Body::Serialize(Json::Value a_root)
{
	Json::Value::Members componentNames = a_root.getMemberNames();
	Json::Value dataRoot = a_root["mApplyFriction"];
	if (!dataRoot.isNull())
		mApplyFriction = dataRoot.asBool();
		
	dataRoot = a_root["mUseGravity"];
	if (!dataRoot.isNull())
		mUseGravity = dataRoot.asBool();

	dataRoot = a_root["mMass"];
	if (!dataRoot.isNull()) {
		mMass = dataRoot.asFloat();
		mInvMass = mMass != 0.0f ? 1.0f / mMass : 0.0f;
	}



}

void Body::DeSerialize(Json::Value& a_root, bool a_ignoreDefault) {
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Body"];

	if (fabsf(defaultRoot["mMass"].asFloat() - mMass) > 0.001f || a_ignoreDefault)
		a_root["mMass"] = mMass;

	if (mApplyFriction != defaultRoot["mApplyFriction"].asBool() || a_ignoreDefault)
		a_root["mApplyFriction"] = mApplyFriction;

	if (mAllowRotations != defaultRoot["mAllowRotations"].asBool())
		a_root["mAllowRotations"] = mAllowRotations;

	/*if (mIsConstantVelocitySet != defaultRoot["mIsConstantVelocitySet"].asBool()) {
		a_root["mIsConstantVelocitySet"] = mApplyFriction;
	}*/

	if (mConstrainX != defaultRoot["mConstrainX"].asBool() || a_ignoreDefault) {
		a_root["mConstrainX"] = mConstrainX;
	}
	if (mConstrainY != defaultRoot["mConstrainY"].asBool() || a_ignoreDefault) {
		a_root["mConstrainY"] = mConstrainY;
	}
	if (mConstrainZ != defaultRoot["mConstrainZ"].asBool() || a_ignoreDefault) {
		a_root["mConstrainZ"] = mConstrainZ;
	}
	if (mUseGravity != defaultRoot["mUseGravity"].asBool() || a_ignoreDefault) {
		a_root["mUseGravity"] = mUseGravity;
	}

}

std::tuple<float, float, float> Body::GetVelocity() {
	return std::tuple<float, float, float>(mVel.x, mVel.y, mVel.z);
}

GEAR_API void Body::SetVelocity(float a_x, float a_y, float a_z)
{
	mVel = glm::vec3(a_x, a_y, a_z);
}

GEAR_API void Body::Clone(int id)
{
	//Entity* temp = ENTITY_MANAGER->GetRuntimeEntity(a_ObjId);
	//_transform = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
	Body *rhs = static_cast<Body*>(COMPONENT_MANAGER->GetComponent<Body>(id, "Body"));

	_transform = rhs->_transform;
	mPos = rhs->mPos;
	mTheta = rhs->mTheta;
	mPrevPos = rhs->mPrevPos;
	mPrevTheta = rhs->mPrevTheta;
	mVel = rhs->mVel;
	mOmega = rhs->mOmega;
	mAcc = rhs->mAcc;
	mAlpha = rhs->mAlpha;
	mLinearForce = rhs->mLinearForce;
	mTorque = rhs->mTorque;
	mMass = rhs->mMass;
	mInvMass = rhs->mInvMass;
	mAllowRotations = rhs->mAllowRotations;
	mI_Body = rhs->mI_Body;
	mInvI_Body = rhs->mInvI_Body;
	mApplyFriction = rhs->mApplyFriction;
	mConstrainX = rhs->mConstrainX;
	mConstrainY = rhs->mConstrainY;
	mConstrainZ = rhs->mConstrainZ;
	mUseGravity = rhs->mUseGravity;
}

GEAR_API glm::mat3 Body::GetGlobalInverseTenser()
{
	glm::mat3 rot = _transform->GetRotationMatrix();
	glm::mat3 inv_rot = glm::transpose(rot);
	return rot * mInvI_Body * inv_rot;
}

GEAR_API void Body::Reset()
{
	_transform = nullptr;
	mPos = glm::vec3(0.0f);
	mPrevPos = glm::vec3(0.0f);
	mVel = glm::vec3(0.0f);
	mAcc = glm::vec3(0.0f);
	mLinearForce = glm::vec3(0.0f);

	mMass = 0.0f;
	mInvMass = 0.0f;
	mApplyFriction = 0.0f;
	mConstrainX = 0.0f;
	mConstrainY = 0.0f;
	mConstrainZ = 0.0f;
	mUseGravity = 0.0f;
	Component::Reset();

}

void Body::HandleEvent(Event * a_pEvent)
{
	if (a_pEvent->GetType() == "PauseEvent")
		mPaused = static_cast<PauseEvent*>(a_pEvent)->mPause;
}
