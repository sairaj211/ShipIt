#include "pch.h"

#include "Managers/Log.h"
#include "Core.h"

#include "CameraManager.h"
#include "Engine.h"
#include "Managers/InputManager.h"
#include "Managers/ComponentManager.h"
#include "Components/Camera.h"
#include "EditCamera.h"
#include "EntityManager.h"

CameraManager* CameraManager::mInstance = nullptr;

CameraManager* CameraManager::GetInstance()
{
	if (!mInstance)
		mInstance = new CameraManager();
	
	return mInstance;
}

void CameraManager::Init()
{
	// Nothing
}

void CameraManager::Update(float a_deltaTime)
{
	if (INPUT_MANAGER->IsTriggered("DefaultCam", -1))
		mpCurrentCamera = nullptr;

	if (!mpCurrentCamera) {
		if (INPUT_MANAGER->IsTriggered(SDL_SCANCODE_F4))
			mCaptureInput = !mCaptureInput;

		if (mCaptureInput)
			mpDefaultCamera->Update(a_deltaTime);
	}
	else if(!ENTITY_MANAGER->GetEntity(mpCurrentCamera->GetOwnerID(), true))
	{
		mpCurrentCamera = nullptr;
	}
}

void CameraManager::Cleanup()
{
	delete mInstance;
	mInstance = nullptr;
}

glm::vec3 CameraManager::GetCameraPosition()
{
	if (mpCurrentCamera)
		return COMPONENT_MANAGER->GetComponent<Transform>(mpCurrentCamera->GetOwnerID(), "Transform")->GetPosition();
	else
		return mpDefaultCamera->GetPosition();
}

std::vector<float> CameraManager::GetVectorCameraPosition()
{
	if (mpCurrentCamera) {
		glm::vec3 position = COMPONENT_MANAGER->GetComponent<Transform>(mpCurrentCamera->GetOwnerID(), "Transform")->GetPosition();
		return std::vector<float>{position.x, position.y, position.z};
	}
	else {
		glm::vec3 position = mpDefaultCamera->GetPosition();
		return std::vector<float>{position.x, position.y, position.z};
	}
}

glm::vec3 CameraManager::GetCameraFront()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetFront();
	else
		return mpDefaultCamera->GetFront();
}

std::vector<float> CameraManager::GetVectorCameraFront()
{
	if (mpCurrentCamera) {
		glm::vec3 position = mpCurrentCamera->GetFront();
		return std::vector<float>{position.x, position.y, position.z};
	}
	else {
		glm::vec3 position = mpDefaultCamera->GetFront();
		return std::vector<float>{position.x, position.y, position.z};
	}
}

glm::vec3 CameraManager::GetCameraUp()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetUp();
	else
		return mpDefaultCamera->GetUp();
}

glm::vec3 CameraManager::GetCameraRight()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetRight();
	else
		return mpDefaultCamera->GetRight();
}

float CameraManager::GetCameraNearPlane()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetNearPlane();
	else
		return mpDefaultCamera->GetNearPlane();
}

float CameraManager::GetCameraFarPlane()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetFarPlane();
	else
		return mpDefaultCamera->GetFarPlane();
}

float CameraManager::GetCameraFieldOfView()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetFieldOfView();
	else
		return mpDefaultCamera->GetFieldOfView();
}

float CameraManager::GetCameraAspectRatio()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetAspectRatio();
	else
		return mpDefaultCamera->GetAspectRatio();
}

glm::mat4 CameraManager::GetCameraViewMatrix()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetViewMatrix();
	else
		return mpDefaultCamera->GetViewMatrix();
}

glm::mat4 CameraManager::GetCameraProjectionMatrix()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetProjectionMatrix();
	else
		return mpDefaultCamera->GetProjectionMatrix();
}

glm::mat4 CameraManager::GetCameraOrthogonalMatrix()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetOrthogonalMatrix();
	else
		return mpDefaultCamera->GetOrthogonalMatrix();
}

void CameraManager::SetCurrentCamera(Camera * a_camera)
{
	mpCurrentCamera = a_camera;
}

int CameraManager::GetCameraID()
{
	if (mpCurrentCamera)
		return mpCurrentCamera->GetOwnerID();
	else
		return -1;
}

CameraManager::CameraManager()
{
	mCaptureInput = true;
	mpCurrentCamera = nullptr;
	mpDefaultCamera = new EditCamera();
}

CameraManager::~CameraManager()
{
	delete mpDefaultCamera;
}