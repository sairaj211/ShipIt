/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
Author: Ivan Cazares, ivan.c, 60004418
- End Header --------------------------------------------------------*/
#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Collider.h"
#include "Physics/Shape.h"
#include "Body.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
#include "Entity.h"

#define COMPONENT_MANAGER ComponentManager::GetInstance()

Collider::Collider() : Component("Collider"), mpShape(nullptr), mIsTrigger(false)
{
	mCoeffRestitution = 0.2f; // bounce
	mCoeffStaticFriction = 0.2f; // to start sliding
	mCoeffDynamicFriction = 0.12f; // while sliding
	mIsCollided = false;
	mWidth = mHeight = mDepth = mRadius = -1.f;
	mOffset = glm::vec3(0.0f);
	mRescale = glm::vec3(0.0f);
}

Collider::~Collider()
{
	if (mpShape)
		delete(mpShape);
};

void Collider::Update(float a_deltaTime) 
{
	mpShape->SetCenter(mpTransform->GetPosition()+mOffset);
	glm::vec3 sizetran = mpTransform->GetScale();
	glm::vec3 size = sizetran + mRescale;
	mpShape->UpdateSize(size);
	glm::mat4 rot = mpTransform->GetRotationMatrix();
	mpShape->SetAxis(rot);	
	

	
};
Component* Collider::Create() { return new  Collider(); };

void Collider::DisplayDebug() {
	ImGui::Checkbox("IsTrigger", &mIsTrigger);
	ImGui::DragFloat3("Offset", &mOffset[0], 0.35f);
	ImGui::DragFloat3("Rescale", &mRescale[0], 0.35f);
	ImGui::DragFloat3("Shape Center", &mpShape->mCenter[0], 0.35f);
}

void Collider::Init() {
	mNodeIndex = -1;
	mpBody = COMPONENT_MANAGER->GetComponent<Body>(GetOwnerID(), "Body", true);
	if (!mpBody)
		mpBody = COMPONENT_MANAGER->GetRuntimeComponent<Body>(GetOwnerID(), "Body");
	mpTransform = COMPONENT_MANAGER->GetComponent<Transform>(GetOwnerID(), "Transform", true);
	if (!mpTransform)
		mpTransform = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(GetOwnerID(), "Transform");
	mpShape->SetCenter(mpTransform->GetPosition());
	if (mRadius > 0) {
		SPHERE* sphere = static_cast<SPHERE*>(mpShape);
		sphere->mRadius = mRadius;
	}
	AABB* cube = static_cast<AABB*>(mpShape);
	if (cube) {
		if (mHeight > 0) {
			cube->mE.y = mHeight;
		}
		if (mWidth > 0) {
			cube->mE.x = mWidth;
		}
		if (mDepth > 0) {

			cube->mE.z = mDepth;
		}
	}
	OBB* obb = static_cast<OBB*>(mpShape);
	if (obb) {
		if (mHeight > 0) {
			obb->mE.y = mHeight;
		}
		if (mWidth > 0) {
			obb->mE.x = mWidth;
		}
		if (mDepth > 0) {
			obb->mE.z = mDepth;
		}
	}
	mpShape->Init();
}


void Collider::Reset()
{
	mLType = "";
	mShape = "";
	mpShape = nullptr;
	mCoeffRestitution = 0.2f; // bounce
	mCoeffStaticFriction = 0.2f; // to start sliding
	mCoeffDynamicFriction = 0.12f; // while sliding
	mIsTrigger = false;
	mIsCollided = false;
	prevScale = glm::vec3(0.0f);
	mOffset = glm::vec3(0.0f);
	mRescale = glm::vec3(0.f);
	mWidth = mHeight = mDepth = mRadius = -1.f;
	
	Component::Reset();
}

void Collider::Initialize(std::string a_type) {

}

void Collider::SelfDelete() {}

void Collider::Serialize(Json::Value a_root)
{
	Json::Value::Members componentNames = a_root.getMemberNames();
	Json::Value dataRoot;
	dataRoot = a_root["mCoeffStaticFriction"];
	if (!dataRoot.isNull()) {
		mCoeffStaticFriction = dataRoot.asFloat();
	}
	dataRoot = a_root["mCoeffDynamicFriction"];
	if (!dataRoot.isNull()) {
		mCoeffDynamicFriction = dataRoot.asFloat();
	}
	dataRoot = a_root["mLType"];
	if (!dataRoot.isNull()) {
		mLType = dataRoot.asString();
	}
	dataRoot = a_root["mShape"];
	if (!dataRoot.isNull()) {
		mShape = dataRoot.asString();

		mpShape = nullptr;
		if (mShape == "AABB")
			mpShape = new AABB(this, mOwnerID);
		else if (mShape == "Sphere")
			mpShape = new SPHERE(this, mOwnerID);
		else if (mShape == "OBB")
			mpShape = new OBB(this, mOwnerID);
		else if (mShape == "Cylinder")
			mpShape = new CYLINDER(this, mOwnerID);
		else if (mShape == "Capsule")
			mpShape = new CAPSULE(this, mOwnerID);
		//TODO customize for the different type of shapes. now is HARDCODED
	}
	dataRoot = a_root["mIsTrigger"];
	if (!dataRoot.isNull()) {
		mIsTrigger = dataRoot.asBool();
	}
	dataRoot = a_root["mWidth"];
	if(!dataRoot.isNull()) {
		mWidth = dataRoot.asFloat();
	}
	dataRoot = a_root["mHeight"];
	if (!dataRoot.isNull()) {
		mHeight = dataRoot.asFloat();
	}
	dataRoot = a_root["mDepth"];
	if (!dataRoot.isNull()) {
		mDepth = dataRoot.asFloat();
	}
	dataRoot = a_root["mRadius"];
	if (!dataRoot.isNull()) {
		mRadius = dataRoot.asFloat();
	}

	dataRoot = a_root["mCoeffRestitution"];
	if (!dataRoot.isNull()) {
		mCoeffRestitution = dataRoot.asFloat();
	}

	dataRoot = a_root["Offset"];
	if (!dataRoot.isNull()) {
		mOffset.x = dataRoot[0].asFloat(); mOffset.y = dataRoot[1].asFloat(); mOffset.z = dataRoot[2].asFloat();
	}

	dataRoot = a_root["Rescale"];
	if (!dataRoot.isNull()) {
		mRescale.x = dataRoot[0].asFloat(); mRescale.y = dataRoot[1].asFloat(); mRescale.z = dataRoot[2].asFloat();
	}
}

void Collider::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Collider"];

	if (mLType != defaultRoot["mLType"].asString() || a_ignoreDefault)
		a_root["mLType"] = mLType;

	if (mShape != defaultRoot["mShape"].asString() || a_ignoreDefault)
		a_root["mShape"] = mShape;

	if (fabsf(defaultRoot["mCoeffStaticFriction"].asFloat() - mCoeffStaticFriction) > 0.001f || a_ignoreDefault)
		a_root["mCoeffStaticFriction"] = mCoeffStaticFriction;

	if (fabsf(defaultRoot["mCoeffDynamicFriction"].asFloat() - mCoeffDynamicFriction) > 0.001f || a_ignoreDefault)
		a_root["mCoeffDynamicFriction"] = mCoeffDynamicFriction;

	if (mIsTrigger != defaultRoot["mIsTrigger"].asBool() || a_ignoreDefault)
		a_root["mIsTrigger"] = mIsTrigger;

	if (mWidth > 0 || a_ignoreDefault)
		a_root["mWidth"] = mWidth;
	if (mHeight > 0 || a_ignoreDefault)
		a_root["mHeight"] = mHeight;
	if(mDepth > 0 || a_ignoreDefault)
		a_root["mDepth"]= mDepth;
	if (mRadius > 0 || a_ignoreDefault)
		a_root["mRadius"] = mRadius;

	if (fabsf(defaultRoot["Rescale"][0].asFloat() - mRescale.x) > 0.001f ||
		fabsf(defaultRoot["Rescale"][1].asFloat() - mRescale.y) > 0.001f ||
		fabsf(defaultRoot["Rescale"][2].asFloat() - mRescale.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value rescale(Json::arrayValue);
		rescale.append(mRescale.x);
		rescale.append(mRescale.y);
		rescale.append(mRescale.z);
		a_root["Rescale"] = rescale;
	}

	if (fabsf(defaultRoot["Offset"][0].asFloat() - mOffset.x) > 0.001f ||
		fabsf(defaultRoot["Offset"][1].asFloat() - mOffset.y) > 0.001f ||
		fabsf(defaultRoot["Offset"][2].asFloat() - mOffset.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value offset(Json::arrayValue);
		offset.append(mOffset.x);
		offset.append(mOffset.y);
		offset.append(mOffset.z);
		a_root["Offset"] = offset;
	}
}

GEAR_API void Collider::HandleEvent(Event * a_pEvent)
{
	
}

GEAR_API void Collider::Clone(int objID)
{
	Collider *rhs = static_cast<Collider*>(COMPONENT_MANAGER->GetComponent<Collider>(objID, "Collider"));

	mLType = rhs->mLType;
	mShape = rhs->mShape;
	mpShape = rhs->mpShape->Clone();
	mpShape->mpOwnerCollider = this;
	mCoeffRestitution = rhs->mCoeffRestitution;
	mCoeffStaticFriction = rhs->mCoeffStaticFriction;
	mCoeffDynamicFriction = rhs->mCoeffDynamicFriction;
	mIsTrigger = rhs->mIsTrigger;
	mIsCollided = rhs->mIsCollided;
	prevScale = rhs->prevScale;
	mWidth = rhs->mWidth;
	mHeight = rhs->mHeight;
	mDepth = rhs->mDepth;
	mRadius = rhs->mRadius;
	mOffset = rhs->mOffset;
	mRescale = rhs->mRescale;
}
