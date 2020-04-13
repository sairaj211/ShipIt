#include "pch.h"

#include "Managers/Log.h"
#include "Core.h"

#include "Camera.h"
#include "Engine.h"
#include "Transform.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
#include "Managers/EntityManager.h"
#include "Managers/CameraManager.h"
#include "Entity.h"

Camera::Camera() :Component("Camera")
{
	mIsDefault = false;
	mFront = glm::vec3(0.0f);
	mUp = glm::vec3(0.0f);
	mRight = glm::vec3(0.0f);

	mNearPlane = 0.1f;
	mFarPlane = 2000.0f;
	mFieldOfView = 90.0f;
	mAspectRatio = 1.0f;

	mViewMatrix = glm::mat4(1.0f);
	mProjectionMatrix = glm::mat4(1.0f);
}

Camera::~Camera()
{

}

void Camera::Reset()
{
	mIsDefault = false;
	mFront = glm::vec3(0.0f);
	mUp = glm::vec3(0.0f);
	mRight = glm::vec3(0.0f);

	mNearPlane = 0.1f;
	mFarPlane = 1000.0f;
	mFieldOfView = 90.0f;
	mAspectRatio = 1.0f;

	mViewMatrix = glm::mat4(1.0f);
	mProjectionMatrix = glm::mat4(1.0f);

	Component::Reset();
}

void Camera::Init()
{
	UpdateCameraAttributes();
}

Camera* Camera::Create()
{ 
	return new Camera();
};

void Camera::Update(float a_deltaTime)
{
	UpdateCameraAttributes();
}

void Camera::DisplayDebug()
{
	ImGui::Checkbox("IsDefault", &mIsDefault);
	ImGui::InputFloat("NearPlane", &mNearPlane);
	ImGui::InputFloat("FarPlane", &mFarPlane);
	ImGui::SliderFloat("FieldOfView", &mFieldOfView, 0.1f, 179.9f, "%.1f");
}

void Camera::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["IsDefault"];
	if (!dataRoot.isNull()) {
		mIsDefault = dataRoot.asBool();
		if (mIsDefault)
			CAMERA_MANAGER->SetCurrentCamera(this);
	}

	dataRoot = a_root["NearPlane"];
	if (!dataRoot.isNull())
		mNearPlane = dataRoot.asFloat();

	dataRoot = a_root["FarPlane"];
	if (!dataRoot.isNull())
		mFarPlane = dataRoot.asFloat();

	dataRoot = a_root["FieldOfView"];
	if (!dataRoot.isNull())
		mFieldOfView = dataRoot.asFloat();
}

void Camera::DeSerialize(Json::Value &a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Camera"];

	if (defaultRoot["IsDefault"].asBool() != mIsDefault || a_ignoreDefault)
		a_root["IsDefault"] = mIsDefault;

	if (fabsf(defaultRoot["NearPlane"].asFloat() - mNearPlane) > 0.001f || a_ignoreDefault)
		a_root["NearPlane"] = mNearPlane;

	if (fabsf(defaultRoot["FarPlane"].asFloat() - mFarPlane) > 0.001f || a_ignoreDefault)
		a_root["FarPlane"] = mFarPlane;

	if (fabsf(defaultRoot["FieldOfView"].asFloat() - mFieldOfView) > 0.001f || a_ignoreDefault)
		a_root["FieldOfView"] = mFieldOfView;
}

void Camera::HandleEvent(Event* a_pEvent)
{

}

void Camera::SetIsDefault(bool a_isDefault)
{
	mIsDefault = a_isDefault;
}

bool Camera::IsDefault()
{
	return mIsDefault;
}

glm::vec3 Camera::GetFront()
{
	return mFront;
}

glm::vec3 Camera::GetUp()
{
	return mUp;
}

glm::vec3 Camera::GetRight()
{
	return mRight;
}

void Camera::SetNearPlane(float a_nearPlane)
{
	if (a_nearPlane < 0.01f)
		mNearPlane = 0.01f;
	else
		mNearPlane = a_nearPlane;

	if (mNearPlane > mFarPlane)
		mFarPlane = mNearPlane + 0.01f;
}

float Camera::GetNearPlane()
{
	return mNearPlane;
}

void Camera::SetFarPlane(float a_farPlane)
{
	if (a_farPlane < 0.02f)
		mFarPlane = 0.02f;
	else
		mNearPlane = a_farPlane;

	if (mFarPlane <= mNearPlane)
		mNearPlane = mFarPlane - 0.01f;
}

float Camera::GetFarPlane()
{
	return mFarPlane;
}

void Camera::SetFieldOfView(float a_fieldOfView)
{
	if (a_fieldOfView < 0.0f)
		mFieldOfView = 0.0f;
	else if (a_fieldOfView > 180.0f)
		mFieldOfView = 180.0f;
	else
		mFieldOfView = a_fieldOfView;
}

float Camera::GetFieldOfView()
{
	return mFieldOfView;
}

float Camera::GetAspectRatio()
{
	return mAspectRatio;
}

glm::mat4 Camera::GetViewMatrix()
{
	return mViewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return mProjectionMatrix;
}

glm::mat4 Camera::GetOrthogonalMatrix()
{
	return mOrthogonalMatrix;

}
void Camera::UpdateCameraAttributes()
{
	// Get Camera's Transform
	Transform* pTr = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform", true);
	if (!pTr)
		pTr = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");

	// Update Camera Vectors
	mFront = glm::normalize(pTr->GetRotationMatrix() * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	mRight = glm::normalize(glm::cross(mFront, glm::vec3(0.0f, 1.0f, 0.0f)));
	mUp = glm::normalize(glm::cross(mRight, mFront));

	float width;
	float height;
	Engine::GetWindowSize(width, height);
	mAspectRatio = width / height;

	glm::vec3 position = pTr->GetPosition();
	mViewMatrix = glm::lookAt(position, position + mFront, glm::vec3(0.0f, 1.0f, 0.0f));
	mProjectionMatrix = glm::perspective(glm::radians(mFieldOfView), mAspectRatio, mNearPlane, mFarPlane);
	mOrthogonalMatrix = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}

GEAR_API void Camera::Clone(int objID)
{
	Camera *rhs = static_cast<Camera*>(COMPONENT_MANAGER->GetComponent<Camera>(objID, "Camera"));

	mIsDefault = rhs->mIsDefault;
	mFront = rhs->mFront;
	mUp = rhs->mUp;
	mRight = rhs->mRight;

	mNearPlane = rhs->mNearPlane;
	mFarPlane = rhs->mFarPlane;
	mFieldOfView = rhs->mFieldOfView;
	mAspectRatio = rhs->mAspectRatio;

	mViewMatrix = rhs->mViewMatrix;
	mProjectionMatrix = rhs->mProjectionMatrix;
}
