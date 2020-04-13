#include "pch.h"

#include "Managers/Log.h"
#include "Core.h"

#include "EditCamera.h"
#include "Engine.h"
#include "Editor/LevelEditor.h"
#include "Managers/InputManager.h"
#include "Input/Mouse.h"

EditCamera::EditCamera()
{
	mPosition = glm::vec3(0.0f);
	mRotationAngles = glm::vec3(0.0f);
	mAspectRatio = 1.0f;
	mNearPlane = 0.1f;
	mFarPlane = 1000.0f;
	mFieldOfView = 60.f;

	UpdateCameraVectors();
	UpdateCameraMatrices();
}

EditCamera::~EditCamera()
{
	
}

void EditCamera::Update(float a_deltaTime)
{

	float moveSpeed = 0.5f;
	float lookSpeed = 0.1f;
	// Fast Mode
	if (INPUT_MANAGER->IsPressed("DefaultCamTurbo", -1))
		moveSpeed *= 10.0f;

	float width;
	float height;
	Engine::GetWindowSize(width, height);
	float hWidth = width * 0.5f;
	float hHeight = height * 0.5f;
	glm::vec2 mousePosition = Mouse::GetInstance()->GetMouseScreenPos();
	if (INPUT_MANAGER->IsTriggered("click_right", -1) && !ImGui::IsAnyWindowHovered()) {
		Engine::HideCursor();
		Engine::MoveCursor(hWidth, hHeight);
		mousePosition = glm::vec2(hWidth, hHeight);
	}
	if (INPUT_MANAGER->IsPressed("click_right", -1) && !ImGui::IsAnyWindowHovered()) {
		Engine::MoveCursor(hWidth, hHeight);
		float dy = hHeight - mousePosition.y;
		float dx = hWidth - mousePosition.x;
		
		mRotationAngles += glm::vec3(dy * lookSpeed, dx * lookSpeed, 0.0f);
		if (mRotationAngles.x >= 90.0f)
			mRotationAngles.x = 89.9f;
		if (mRotationAngles.x <= -90.0f)
			mRotationAngles.x = -89.9f;
	}
	else {
		if (INPUT_MANAGER->IsPressed("click_middle", -1)) {
			

			glm::vec2 newMousePosition = mousePosition;
			bool changed = false;
			if (newMousePosition.x >= (width - 1)) {
				newMousePosition.x = 1.0f;
				changed = true;
			}
			else if (newMousePosition.x <= 0.0f) {
				newMousePosition.x = width - 2;
				changed = true;
			}
			if (newMousePosition.y >= (height - 1)) {
				newMousePosition.y = 1.0f;
				changed = true;
			}
			else if (newMousePosition.y <= 0.0f) {
				newMousePosition.y = height - 2;
				changed = true;
			}
			if (changed) {
				Engine::MoveCursor(newMousePosition.x, newMousePosition.y);
			}
			glm::vec2 prevMousePosition = Mouse::GetInstance()->GetPrevMouseScreenPos();
			
			if (prevMousePosition.x >= (width - 2)) {
				prevMousePosition.x = 0.0f;
			}
			else if (prevMousePosition.x <= 1.0f) {
				prevMousePosition.x = width - 1;
			}
			if (prevMousePosition.y >= (height - 2)) {
				prevMousePosition.y = 0.0f;
			}
			else if (prevMousePosition.y <= 1.0f) {
				prevMousePosition.y = height - 1;
			}
			glm::vec2 mouseDeltas = (prevMousePosition - mousePosition) * 0.01f;
			mPosition -= mouseDeltas.y * mUp;
			mPosition += mouseDeltas.x * mRight;
		}
	}
	if (INPUT_MANAGER->IsReleased("click_right", -1))
		Engine::ShowCursor();

	UpdateCameraVectors();

	if (Mouse::GetInstance()->ScrollUp() && !ImGui::IsAnyWindowHovered()) {
		mPosition += 2.0f * moveSpeed * mFront;
	}
	if (Mouse::GetInstance()->ScrollDown() && !ImGui::IsAnyWindowHovered()) {
		mPosition -= 2.0f* moveSpeed * mFront;
	}

	if (INPUT_MANAGER->IsPressed("click_right", -1)) {
		if (INPUT_MANAGER->IsPressed("DefaultCamFW", -1))
			mPosition += moveSpeed * mFront;
		if (INPUT_MANAGER->IsPressed("DefaultCamBK", -1))
			mPosition -= moveSpeed * mFront;
		if (INPUT_MANAGER->IsPressed("DefaultCamLT", -1))
			mPosition -= moveSpeed * mRight;
		if (INPUT_MANAGER->IsPressed("DefaultCamRT", -1))
			mPosition += moveSpeed * mRight;
		if (INPUT_MANAGER->IsPressed("DefaultCamUP", -1))
			mPosition += moveSpeed * mUp;
		if (INPUT_MANAGER->IsPressed("DefaultCamDN", -1))
			mPosition -= moveSpeed * mUp;
	}

	UpdateCameraMatrices();
}

glm::vec3 EditCamera::GetPosition()
{
	return mPosition;
}

glm::vec3 EditCamera::GetFront()
{
	return mFront;
}

glm::vec3 EditCamera::GetUp()
{
	return mUp;
}

glm::vec3 EditCamera::GetRight()
{
	return mRight;
}

float EditCamera::GetNearPlane()
{
	return mNearPlane;
}


float EditCamera::GetFarPlane()
{
	return mFarPlane;
}

float EditCamera::GetFieldOfView()
{
	return mFieldOfView;
}

float EditCamera::GetAspectRatio()
{
	return mAspectRatio;
}

glm::mat4 EditCamera::GetViewMatrix()
{
	return mViewMatrix;
}

glm::mat4 EditCamera::GetProjectionMatrix()
{
	return mProjectionMatrix;
}

glm::mat4 EditCamera::GetOrthogonalMatrix()
{
	return mOrthogonalMatrix;
}

void EditCamera::UpdateCameraVectors()
{
	mFront = glm::normalize(glm::toMat4(glm::quat(mRotationAngles * (glm::pi<float>() / 180.0f))) * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	mRight = glm::normalize(glm::cross(mFront, glm::vec3(0.0f, 1.0f, 0.0f)));
	mUp = glm::normalize(glm::cross(mRight, mFront));
}

void EditCamera::UpdateCameraMatrices()
{
	float width;
	float height;
	Engine::GetWindowSize(width, height);
	mAspectRatio = width / height;
	mViewMatrix = glm::lookAt(mPosition, mPosition + mFront, glm::vec3(0.0f, 1.0f, 0.0f));
	mProjectionMatrix = glm::perspective(glm::radians(mFieldOfView), mAspectRatio, mNearPlane, mFarPlane);
	mOrthogonalMatrix = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
}