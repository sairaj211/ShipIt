/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
Author: Ivan Cazares, ivan.c, 60004418
- End Header --------------------------------------------------------*/

#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Transform.h"
#include "../Managers/ResourceManager.h"
#include "../Resources/JsonData.h"
#include "../Managers/EntityManager.h"
#include "Managers/ComponentManager.h"
#include "../Entity.h"

Transform::Transform() : Component("Transform")
{
	mPosition = glm::vec3(0.0);
	mScale = glm::vec3(1.0f);
	mRotationAngles = glm::vec3(0.0f);

	mLocalPosition = mPosition;
	mLocalScale = mScale;
	mLocalRotationAngles = mRotationAngles;

	mDirty = POSITION_BIT | ROTATION_BIT | SCALE_BIT;
}

Transform::~Transform()
{

}

void Transform::Reset()
{
	mPosition = glm::vec3(0.0);
	mScale = glm::vec3(1.0f);
	mRotationAngles = glm::vec3(0.0f);

	mLocalPosition = mPosition;
	mLocalScale = mScale;
	mLocalRotationAngles = mRotationAngles;

	mDirty = POSITION_BIT | ROTATION_BIT | SCALE_BIT;

	// Reset values of base component
	Component::Reset();
}

Transform* Transform::Create()
{
	return new Transform();
}

std::vector<float> Transform::GetTuplePosition()
{
	glm::vec3 pos = GetPosition();
	return std::vector<float>{pos.x, pos.y, pos.z};
}

void Transform::SetPositionValues(float a_x, float a_y, float a_z)
{
	SetPosition(glm::vec3(a_x, a_y, a_z));
}

void Transform::SetLocalPositionValues(float a_x, float a_y, float a_z)
{
	SetLocalPosition(glm::vec3(a_x, a_y, a_z));
}

std::vector<float> Transform::GetLocalTuplePosition() {
	glm::vec3 local = GetLocalPosition();
	return std::vector<float>{local.x, local.y, local.z};
}


std::vector<float> Transform::GetTupleScale()
{
	glm::vec3 scale = GetScale();
	return std::vector<float>{scale.x, scale.y, scale.z};
}

void Transform::SetScaleValues(float a_x, float a_y, float a_z)
{
	SetScale(glm::vec3(a_x, a_y, a_z));
}

void Transform::SetLocalScaleValues(float a_x, float a_y, float a_z)
{
	SetLocalScale(glm::vec3(a_x, a_y, a_z));
}


std::vector<float> Transform::GetLocalTupleScale()
{
	glm::vec3 scale = GetLocalScale();
	return std::vector<float>{scale.x, scale.y, scale.z};
}

void Transform::Update(float a_deltaTime)
{
	
}

void Transform::DisplayDebug()
{
	glm::vec3 prevPos = mLocalPosition;
	glm::vec3 prevScale = mLocalScale;
	glm::vec3 prevRot = mLocalRotationAngles;

	ImGui::DragFloat3("Position", &mLocalPosition[0],0.35f);
	ImGui::DragFloat3("Scale", &mLocalScale[0],0.35f);
	ImGui::DragFloat3("RotationAngles", &mLocalRotationAngles[0],0.35f);

	if (!glm::all(glm::equal(prevPos, mLocalPosition)))
		SetLocalPosition(mLocalPosition);

	if (!glm::all(glm::equal(prevScale, mLocalScale)))
		SetLocalScale(mLocalScale);

	if (!glm::all(glm::equal(prevRot, mLocalRotationAngles)))
		SetLocalRotationAngles(mLocalRotationAngles);
}

glm::mat4 Transform::ComputeModelMatrix()
{
	// Initialize to identity
	glm::mat4 modelMatrix(1.0f);

	// Convert to Radians
	glm::vec3 radianAngles = GetRotationAngles() * (glm::pi<float>() / 180.0f);

	// Compute T * R * S
	mQuaterions = glm::quat(radianAngles);
	modelMatrix = glm::translate(modelMatrix, GetPosition()) * glm::toMat4(mQuaterions) * glm::scale(glm::mat4(1.0f), GetScale());

	return modelMatrix;
}

void Transform::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["Scale"];
	if (!dataRoot.isNull())
		SetLocalScale(glm::vec3(dataRoot[0].asFloat(), dataRoot[1].asFloat(), dataRoot[2].asFloat()));

	dataRoot = a_root["Translate"];
	if (!dataRoot.isNull())
		SetLocalPosition(glm::vec3(dataRoot[0].asFloat(), dataRoot[1].asFloat(), dataRoot[2].asFloat()));

	dataRoot = a_root["RotationAngles"];
	if (!dataRoot.isNull())
		SetLocalRotationAngles(glm::vec3(dataRoot[0].asFloat(), dataRoot[1].asFloat(), dataRoot[2].asFloat()));
}

void Transform::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Transform"];

	// TODO: Make an IsEqual function for floats
	if (fabsf(defaultRoot["Translate"][0].asFloat() - mLocalPosition.x) > 0.001f ||
		fabsf(defaultRoot["Translate"][1].asFloat() - mLocalPosition.y) > 0.001f ||
		fabsf(defaultRoot["Translate"][2].asFloat() - mLocalPosition.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value translate(Json::arrayValue);
		translate.append(mLocalPosition.x);
		translate.append(mLocalPosition.y);
		translate.append(mLocalPosition.z);
		a_root["Translate"] = translate;
	}
	
	if (fabsf(defaultRoot["Scale"][0].asFloat() - mLocalScale.x) > 0.001f ||
		fabsf(defaultRoot["Scale"][1].asFloat() - mLocalScale.y) > 0.001f ||
		fabsf(defaultRoot["Scale"][2].asFloat() - mLocalScale.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value scale(Json::arrayValue);
		scale.append(mLocalScale.x);
		scale.append(mLocalScale.y);
		scale.append(mLocalScale.z);
		a_root["Scale"] = scale;
	}
	
	if (fabsf(defaultRoot["RotationAngles"][0].asFloat() - mLocalRotationAngles.x) > 0.001f ||
		fabsf(defaultRoot["RotationAngles"][1].asFloat() - mLocalRotationAngles.y) > 0.001f ||
		fabsf(defaultRoot["RotationAngles"][2].asFloat() - mLocalRotationAngles.z) > 0.001f ||
		a_ignoreDefault) {
		Json::Value rotationAngles(Json::arrayValue);
		rotationAngles.append(mLocalRotationAngles.x);
		rotationAngles.append(mLocalRotationAngles.y);
		rotationAngles.append(mLocalRotationAngles.z);
		a_root["RotationAngles"] = rotationAngles;
	}
}

void Transform::OnSetParent()
{
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
	Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (!pParentTr)
		pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (pParentTr) {
		mLocalPosition = glm::vec3(glm::inverse(pParentTr->ComputeModelMatrix()) * glm::vec4(mLocalPosition, 1.0f));
		mLocalScale /= pParentTr->GetScale();
		mLocalRotationAngles -= pParentTr->GetRotationAngles();
		//
		glm::vec3 radianAngles = mLocalRotationAngles * (glm::pi<float>() / 180.0f);
		mQuaterions = glm::quat(radianAngles);
	}
}

void Transform::OnRemoveParent()
{
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
	Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (!pParentTr)
		pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (pParentTr) {
		mLocalPosition = glm::vec3(pParentTr->ComputeModelMatrix() * glm::vec4(mLocalPosition, 1.0f));
		mLocalScale *= pParentTr->GetScale();
		mLocalRotationAngles += pParentTr->GetRotationAngles();
		glm::vec3 radianAngles = mLocalRotationAngles * (glm::pi<float>() / 180.0f);
		mQuaterions = glm::quat(radianAngles);
	}
}

void Transform::SetDirty(int a_dirtyBits) {
	if (a_dirtyBits < 0 || a_dirtyBits > (POSITION_BIT | ROTATION_BIT | SCALE_BIT))
		a_dirtyBits = (POSITION_BIT | ROTATION_BIT | SCALE_BIT);

	mDirty |= a_dirtyBits;
}

void Transform::SetPosition(glm::vec3 a_position)
{
	glm::vec pos = a_position;
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
	Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (!pParentTr)
		pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (pParentTr)
		pos = glm::vec3(glm::inverse(pParentTr->ComputeModelMatrix()) * glm::vec4(pos, 1.0f));
	SetLocalPosition(pos); // TODO: Optimize
}

glm::vec3 Transform::GetPosition()
{
	if (mDirty & POSITION_BIT) {
		Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
		if (!pEntity)
			pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
		glm::mat4 parentTransform = glm::mat4(1.0f);
		Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
		if (!pParentTr)
			pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
		if (pParentTr)
			parentTransform = pParentTr->ComputeModelMatrix();
		mPosition = glm::vec3(parentTransform * glm::vec4(mLocalPosition, 1.0f));
		mDirty &= ~POSITION_BIT;
	}

	return mPosition;
}

void Transform::SetLocalPosition(glm::vec3 a_position)
{
	mLocalPosition = a_position;
	mDirty |= POSITION_BIT;

	// Set every child Entity's Transform to dirty
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
	pEntity->SetChildrenDirty<Transform>("Transform", POSITION_BIT);
}

glm::vec3 Transform::GetLocalPosition()
{
	return mLocalPosition;
}

void Transform::SetRotationAngleValues(float a_x, float a_y, float a_z)
{
	SetRotationAngles(glm::vec3(a_x, a_y, a_z));
}

std::vector<float> Transform::GetRotationAngleValues()
{
	glm::vec3 rot = GetRotationAngles();
	return std::vector<float>{rot.x, rot.y, rot.z};
}

void Transform::SetRotationAngles(glm::vec3 a_rotationAngles)
{
	glm::vec rot = a_rotationAngles;
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
	Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (!pParentTr)
		pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (pParentTr)
		rot -= pParentTr->GetRotationAngles();
	SetLocalRotationAngles(rot); // TODO: Optimize
}

glm::vec3 Transform::GetRotationAngles()
{
	if (mDirty & ROTATION_BIT) {
		Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
		if (!pEntity)
			pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
		glm::vec3 parentRotationAngles = glm::vec3(0.0f);
		Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
		if (!pParentTr)
			pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
		if (pParentTr)
			parentRotationAngles = pParentTr->GetRotationAngles();
		mRotationAngles = parentRotationAngles + mLocalRotationAngles;
		mDirty &= ~ROTATION_BIT;
	}

	return mRotationAngles;
}

GEAR_API void Transform::SetRotationQuaterions(glm::quat a_rotationQuat)
{
	glm::quat q = a_rotationQuat;
	glm::vec3 rot = glm::eulerAngles(q) * (180.f/ glm::pi<float>());
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
	Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (!pParentTr)
		pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (pParentTr)
		rot -= pParentTr->GetRotationAngles();
	SetLocalRotationAngles(rot); // TODO: Optimize
}

GEAR_API glm::quat Transform::GetRotationQuaterions()
{
	if (mDirty & ROTATION_BIT) {
		Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
		if (!pEntity)
			pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
		glm::vec3 parentRotationAngles = glm::vec3(0.0f);
		Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
		if (!pParentTr)
			pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
		if (pParentTr)
			parentRotationAngles = pParentTr->GetRotationAngles();
		mRotationAngles = parentRotationAngles + mLocalRotationAngles;
		mDirty &= ~ROTATION_BIT;
		glm::vec3 radianAngles = mRotationAngles * (glm::pi<float>() / 180.0f);
		mQuaterions = glm::quat(radianAngles);
	}

	return mQuaterions;
}

void Transform::SetLocalRotationAngles(glm::vec3 a_rotationAngles)
{
	mLocalRotationAngles = a_rotationAngles;
	glm::vec3 radianAngles = mLocalRotationAngles * (glm::pi<float>() / 180.0f);

	// Compute T * R * S
	mQuaterions =  glm::quat(radianAngles);
	mDirty |= ROTATION_BIT;

	// Set every child Entity's Transform to dirty
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
	pEntity->SetChildrenDirty<Transform>("Transform", ROTATION_BIT | POSITION_BIT);
}

glm::vec3 Transform::GetLocalRotationAngles()
{
	return mLocalRotationAngles;
}

void Transform::SetScale(glm::vec3 a_scale)
{
	glm::vec scale = a_scale;
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
	Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (!pParentTr)
		pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
	if (pParentTr)
		scale /= pParentTr->GetScale();
	SetLocalScale(scale); // TODO: Optimize
}

glm::vec3 Transform::GetScale()
{
	if (mDirty & SCALE_BIT) {
		Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
		if (!pEntity)
			pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
		glm::vec3 parentScale = glm::vec3(1.0f);
		Transform* pParentTr = COMPONENT_MANAGER->GetComponent<Transform>(pEntity->GetParentID(), "Transform");
		if (!pParentTr)
			pParentTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(pEntity->GetParentID(), "Transform");
		if (pParentTr)
			parentScale = pParentTr->GetScale();
		mScale = parentScale * mLocalScale;
		mDirty &= ~SCALE_BIT;
	}

	return mScale;
}

void Transform::SetLocalScale(glm::vec3 a_scale)
{
	mLocalScale = a_scale;
	mDirty |= SCALE_BIT;

	// Set every child Entity's Transform to dirty
	Entity* pEntity = ENTITY_MANAGER->GetEntity(mOwnerID, true);
	if (!pEntity)
		pEntity = ENTITY_MANAGER->GetRuntimeEntity(mOwnerID);
	pEntity->SetChildrenDirty<Transform>("Transform", SCALE_BIT | POSITION_BIT);
}

glm::vec3 Transform::GetLocalScale()
{
	return mLocalScale;
}

GEAR_API void Transform::HandleEvent(Event * a_pEvent)
{

}

GEAR_API glm::mat4 Transform::GetRotationMatrix()
{
	glm::vec3 radianAngles = GetRotationAngles() * (glm::pi<float>() / 180.0f);

	// Compute T * R * S
	mQuaterions = glm::quat(radianAngles);
	return glm::toMat4(mQuaterions);
}

GEAR_API glm::mat4 Transform::GetLocalRotationMatrix()
{
	return GetRotationMatrix();
}

GEAR_API void Transform::Clone(int objID)
{
	Transform *rhs = static_cast<Transform*>(COMPONENT_MANAGER->GetComponent<Transform>(objID, "Transform"));

	mPosition = rhs->mPosition;
	mScale = rhs->mScale;
	mRotationAngles = rhs->mRotationAngles;
	mQuaterions = rhs->mQuaterions;

	mLocalPosition = rhs->mLocalPosition;
	mLocalScale = rhs->mLocalScale;
	mLocalRotationAngles = rhs->mLocalRotationAngles;
}
