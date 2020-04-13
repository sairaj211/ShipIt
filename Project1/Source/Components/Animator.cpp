#include "pch.h"
#include "Managers/Log.h"

#include "Core.h"
#include "Animator.h"
#include "Transform.h"
#include "Managers/ComponentManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/EntityManager.h"
#include "Resources/AffineAnimation.h"
#include "Resources/SkeletalAnimation.h"
#include "Entity.h"
#include "Managers/Log.h"
#include "Resources/JsonData.h"
#include "Resources/Model.h"
#include "Skeleton.h"
#include "Timer.h"

AffineState::AffineState()
{
	stateName = "None";
	pAffineAnimation = nullptr;
	blendDuration = 0.3f;
	returnToDefault = true;
}

SkeletalState::SkeletalState()
{
	stateName = "None";
	pSkeletalAnimation = nullptr;
	blendDuration = 0.3f;
	returnToDefault = true;
}

Animator::Animator() : Component("Animator") 
{
	mCurrentAffineTime = 0.0f;
	mNextAffineTime = 0.0f;
	mAffineBlendingTime = 1.0f;
	mpCurrentAffineState = nullptr;
	mpNextAffineState = nullptr;
	mpRuntimeAffineState = nullptr;
	mDefaultAffineState = "None";
	mAffineAnimationMatrix = glm::mat4(1.0f);
	mComputeAffineAnimation = true;

	mCurrentSkeletalTime = 0.0f;
	mNextSkeletalTime = 0.0f;
	mSkeletalBlendingTime = 1.0f;
	mpCurrentSkeletalState = nullptr;
	mpNextSkeletalState = nullptr;
	mpRuntimeSkeletalState = nullptr;
	mDefaultSkeletalState = "None";
	mComputeSkeletalAnimation = true;

	prevScale = glm::vec3(1.0f);
	prevRotation = glm::quat(glm::vec3(0.0f));
	prevTranslation = glm::vec3(0.0f);

	prevNextScale = glm::vec3(1.0f);
	prevNextRotation = glm::quat(glm::vec3(0.0f));
	prevNextTranslation = glm::vec3(0.0f);

	mPause = false;
	mSpeedModifier = 1.0f;
}

Animator::~Animator() 
{

}

void Animator::Reset() 
{
	mCurrentAffineTime = 0.0f;
	mNextAffineTime = 0.0f;
	mAffineBlendingTime = 1.0f;
	mpCurrentAffineState = nullptr;
	mpNextAffineState = nullptr;
	mpRuntimeAffineState = nullptr;
	mDefaultAffineState = "None";
	mAffineAnimationMatrix = glm::mat4(1.0f);
	mComputeAffineAnimation = true;

	mCurrentSkeletalTime = 0.0f;
	mNextSkeletalTime = 0.0f;
	mSkeletalBlendingTime = 1.0f;
	mpCurrentSkeletalState = nullptr;
	mpNextSkeletalState = nullptr;
	mpRuntimeSkeletalState = nullptr;
	mDefaultSkeletalState = "None";
	mSkeletalTransformations.clear();
	mComputeSkeletalAnimation = true;

	prevScale = glm::vec3(1.0f);
	prevRotation = glm::quat(glm::vec3(0.0f));
	prevTranslation = glm::vec3(0.0f);

	prevNextScale = glm::vec3(1.0f);
	prevNextRotation = glm::quat(glm::vec3(0.0f));
	prevNextTranslation = glm::vec3(0.0f);

	mPause = false;
	mSpeedModifier = 1.0f;

	for (auto& element : mAffineStateMap) {
		delete element.second;
	}
	mAffineStateMap.clear();

	for (auto& element : mSkeletalStateMap) {
		delete element.second;
	}
	mSkeletalStateMap.clear();

	// Reset values of base component
	Component::Reset();
}

Animator* Animator::Create() 
{
	return new Animator();
}

void Animator::Update(float a_deltaTime) 
{
	float adjustedDeltaTime = (a_deltaTime / 1000.0f) * mSpeedModifier;

	UpdateAffineAnimation(adjustedDeltaTime);
	UpdateSkeletalAnimation(adjustedDeltaTime);

	mComputeAffineAnimation = true;
	mComputeSkeletalAnimation = true;
}

void Animator::DisplayDebug() 
{
	ImGui::Checkbox("Pause", &mPause);

	if (mpCurrentAffineState) {
		ImGui::DragFloat("AffineTime", &mCurrentAffineTime, 0.01f, 0.0f, mpCurrentAffineState->pAffineAnimation->mDuration - 0.01f, "%.2f");
	}
	
	if (mpCurrentSkeletalState) {
		ImGui::DragFloat("SkeletalTime", &mCurrentSkeletalTime, 0.01f, 0.0f, mpCurrentSkeletalState->pSkeletalAnimation->mDuration - 0.01f, "%.2f");
	}

	// Get affine state list
	std::vector<std::string> affineStates;
	affineStates.push_back("None");
	for (auto& element : mAffineStateMap) {
		affineStates.push_back(element.first);
	}

	// List available states for the current state
	std::string currentStateName = "None";
	if (mpCurrentAffineState) {
		currentStateName = mpCurrentAffineState->stateName;
	}
	if (ImGui::BeginCombo("CurrentAffineState##CurrentAffineState", currentStateName.c_str())) {
		for (int i = 0; i < affineStates.size(); ++i) {
			bool selected = currentStateName == affineStates[i];
			if (ImGui::Selectable(affineStates[i].c_str(), selected)) {
				ChangeAffineState(affineStates[i]);
			}
			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// List available states for the default affine state
	if (ImGui::BeginCombo("DefaultAffineState##DefaultAffineState", mDefaultAffineState.c_str())) {
		for (int i = 0; i < affineStates.size(); ++i) {
			bool selected = mDefaultAffineState == affineStates[i];
			if (ImGui::Selectable(affineStates[i].c_str(), selected)) {
				std::string prevDefaultState = mDefaultAffineState;
				mDefaultAffineState = affineStates[i];

				std::string currentStateName = "None";
				if (mpCurrentAffineState) {
					currentStateName = mpCurrentAffineState->stateName;
				}
				if (prevDefaultState == currentStateName) {
					ChangeAffineState(mDefaultAffineState);
				}
			}
			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// Get skeletal state list
	std::vector<std::string> skeletalStates;
	skeletalStates.push_back("None");
	for (auto& element : mSkeletalStateMap) {
		skeletalStates.push_back(element.first);
	}

	// List available states for the current state
	currentStateName = "None";
	if (mpCurrentSkeletalState) {
		currentStateName = mpCurrentSkeletalState->stateName;
	}
	if (ImGui::BeginCombo("CurrentSkeletalState##CurrentSkeletalState", currentStateName.c_str())) {
		for (int i = 0; i < skeletalStates.size(); ++i) {
			bool selected = currentStateName == skeletalStates[i];
			if (ImGui::Selectable(skeletalStates[i].c_str(), selected)) {
				ChangeSkeletalState(skeletalStates[i]);
			}
			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// List available states for the default skeletal state
	if (ImGui::BeginCombo("DefaultSkeletalState##DefaultSkeletalState", mDefaultSkeletalState.c_str())) {
		for (int i = 0; i < skeletalStates.size(); ++i) {
			bool selected = mDefaultSkeletalState == skeletalStates[i];
			if (ImGui::Selectable(skeletalStates[i].c_str(), selected)) {
				std::string prevDefaultState = mDefaultSkeletalState;
				mDefaultSkeletalState = skeletalStates[i];

				std::string currentStateName = "None";
				if (mpCurrentSkeletalState) {
					currentStateName = mpCurrentSkeletalState->stateName;
				}
				if (prevDefaultState == currentStateName) {
					ChangeSkeletalState(mDefaultSkeletalState);
				}
			}
			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	ImGui::DragFloat("SpeedModifier", &mSpeedModifier, 0.1f, 0.0f, 10.0f, "%.1f");
}

void Animator::Serialize(Json::Value a_root) 
{
	Json::Value dataRoot = a_root["AffineStates"];
	if (!dataRoot.isNull()) {
		mAffineStateMap.clear();
		for (unsigned int i = 0; i < dataRoot.size(); ++i) {
			Json::Value state = dataRoot[i];
			if (state["StateName"].isNull() || 
				state["AffineAnimationName"].isNull() ||
				state["BlendDuration"].isNull() ||
				state["ReturnToDefault"].isNull()) {
				continue;
			}

			// Set State Data
			AffineState* pAffineState = new AffineState();
			pAffineState->stateName = state["StateName"].asString();
			pAffineState->pAffineAnimation = ResourceManager::GetInstance()->LoadAffineAnimation(state["AffineAnimationName"].asString());
			pAffineState->blendDuration = state["BlendDuration"].asFloat();
			pAffineState->returnToDefault = state["ReturnToDefault"].asBool();

			mAffineStateMap[pAffineState->stateName] = pAffineState;
		}
	}

	dataRoot = a_root["DefaultAffineState"];
	if (!dataRoot.isNull()) {
		if (dataRoot.asString() == "None" || mAffineStateMap.find(dataRoot.asString()) == mAffineStateMap.end()) {
			mDefaultAffineState = "None";
			mpCurrentAffineState = nullptr;
		}
		else {
			mDefaultAffineState = dataRoot.asString();
			mpCurrentAffineState = mAffineStateMap[mDefaultAffineState];
		}
	}

	dataRoot = a_root["SkeletalStates"];
	if (!dataRoot.isNull()) {
		mSkeletalStateMap.clear();
		for (unsigned int i = 0; i < dataRoot.size(); ++i) {
			Json::Value state = dataRoot[i];
			if (state["StateName"].isNull() ||
				state["SkeletalAnimationName"].isNull() ||
				state["BlendDuration"].isNull() ||
				state["ReturnToDefault"].isNull()) {
				continue;
			}

			// Set State Data
			SkeletalState* pSkeletalState = new SkeletalState();
			pSkeletalState->stateName = state["StateName"].asString();
			pSkeletalState->pSkeletalAnimation = ResourceManager::GetInstance()->LoadSkeletalAnimation(state["SkeletalAnimationName"].asString());
			pSkeletalState->blendDuration = state["BlendDuration"].asFloat();
			pSkeletalState->returnToDefault = state["ReturnToDefault"].asBool();

			mSkeletalStateMap[pSkeletalState->stateName] = pSkeletalState;
		}
	}

	dataRoot = a_root["DefaultSkeletalState"];
	if (!dataRoot.isNull()) {
		if (dataRoot.asString() == "None" || mSkeletalStateMap.find(dataRoot.asString()) == mSkeletalStateMap.end()) {
			mDefaultSkeletalState = "None";
			mpCurrentSkeletalState = nullptr;
		}
		else {
			mDefaultSkeletalState = dataRoot.asString();
			mpCurrentSkeletalState = mSkeletalStateMap[mDefaultSkeletalState];
		}
	}
}

void Animator::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["Animator"];

	if (mAffineStateMap.find("None") != mAffineStateMap.end()) {
		mAffineStateMap.erase("None");
	}

	// Check if there is a difference with the AffineStates
	bool stateDifference = false;
	if (mAffineStateMap.size() != defaultRoot["AffineStates"].size()) {
		stateDifference = true;
	}
	else {
		for (unsigned int i = 0; i < defaultRoot["AffineStates"].size(); ++i) {
			Json::Value defaultState = defaultRoot["AffineStates"][i];
			
			// Check if state name differs
			if (mAffineStateMap.find(defaultState["StateName"].asString()) == mAffineStateMap.end()) {
				stateDifference = true;
				break;
			}
			else {
				AffineState* pAffineState = mAffineStateMap[defaultState["StateName"].asString()];
			
				// Check attributes of state for differences
				if (pAffineState->pAffineAnimation->mName != defaultState["AffineAnimationName"].asString()) {
					stateDifference = true;
					break;
				}
				else if (fabsf(defaultState["BlendDuration"].asFloat() - pAffineState->blendDuration) >= 0.001f) {
					stateDifference = true;
					break;
				}
				else if (pAffineState->returnToDefault != defaultState["ReturnToDefault"].asBool()) {
					stateDifference = true;
					break;
				}
			}
		}
	}

	if (stateDifference || a_ignoreDefault) {
		Json::Value states(Json::arrayValue);
		for (auto& element : mAffineStateMap) {
			Json::Value state;
			state["StateName"] = element.first;
			state["AffineAnimationName"] = element.second->pAffineAnimation->mName;
			state["BlendDuration"] = element.second->blendDuration;
			state["ReturnToDefault"] = element.second->returnToDefault;
			states.append(state);
		}
		a_root["AffineStates"] = states;
	}

	if (mDefaultAffineState != defaultRoot["DefaultAffineState"].asString() || a_ignoreDefault)
		a_root["DefaultAffineState"] = mDefaultAffineState;

	if (mSkeletalStateMap.find("None") != mSkeletalStateMap.end()) {
		mSkeletalStateMap.erase("None");
	}

	// Check if there is a difference with the SkeletalStates
	stateDifference = false;
	if (mSkeletalStateMap.size() != defaultRoot["SkeletalStates"].size()) {
		stateDifference = true;
	}
	else {
		for (unsigned int i = 0; i < defaultRoot["SkeletalStates"].size(); ++i) {
			Json::Value defaultState = defaultRoot["SkeletalStates"][i];

			// Check if state name differs
			if (mSkeletalStateMap.find(defaultState["StateName"].asString()) == mSkeletalStateMap.end()) {
				stateDifference = true;
				break;
			}
			else {
				SkeletalState* pSkeletalState = mSkeletalStateMap[defaultState["StateName"].asString()];

				// Check attributes of state for differences
				if (pSkeletalState->pSkeletalAnimation->mName != defaultState["SkeletalAnimationName"].asString()) {
					stateDifference = true;
					break;
				}
				else if (fabsf(defaultState["BlendDuration"].asFloat() - pSkeletalState->blendDuration) >= 0.001f) {
					stateDifference = true;
					break;
				}
				else if (pSkeletalState->returnToDefault != defaultState["ReturnToDefault"].asBool()) {
					stateDifference = true;
					break;
				}
			}
		}
	}

	if (stateDifference || a_ignoreDefault) {
		Json::Value states(Json::objectValue);
		for (auto& element : mSkeletalStateMap) {
			Json::Value state;
			state["StateName"] = element.first;
			state["SkeletalAnimationName"] = element.second->pSkeletalAnimation->mName;
			state["BlendDuration"] = element.second->blendDuration;
			state["ReturnToDefault"] = element.second->returnToDefault;
			states.append(state);
		}
		a_root["SkeletalStates"] = states;
	}

	if (mDefaultSkeletalState != defaultRoot["DefaultSkeletalState"].asString() || a_ignoreDefault)
		a_root["DefaultSkeletalState"] = mDefaultSkeletalState;
}

glm::mat4 Animator::GetAffineAnimationMatrix()
{
	glm::mat4 affineAnimationMatrix = glm::mat4(1.0f);
	
	// Reset Affine Transformation Matrix if there is no current animation
	if (!mpCurrentAffineState)
		mAffineAnimationMatrix = affineAnimationMatrix;

	// Interpolate Between Keyframes to get Affine Transformation Matrix
	if (mpCurrentAffineState && mComputeAffineAnimation) {
		Transform* pTr = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
		// Blend Animations
		if (mpNextAffineState) {
			AffineAnimation* pCurrentAffineAnimation = mpCurrentAffineState->pAffineAnimation;
			AffineAnimation* pNextAffineAnimation = mpNextAffineState->pAffineAnimation;

			// Scale
			glm::vec3 scale0 = pCurrentAffineAnimation->InterpolateScale(mCurrentAffineTime);
			glm::vec3 scale1 = pNextAffineAnimation->InterpolateScale(mNextAffineTime);
			glm::vec3 scale = glm::lerp(scale0, scale0 * scale1, mAffineBlendingTime);
			glm::vec3 realScale = pTr->GetScale();
			realScale /= prevScale;
			pTr->SetScale(realScale * scale);
			prevScale = scale;
			prevNextScale = scale1;
			//glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

			// Rotate
			glm::quat rotate0 = pCurrentAffineAnimation->InterpolateRotate(mCurrentAffineTime);
			glm::quat rotate1 = pNextAffineAnimation->InterpolateRotate(mNextAffineTime);
			glm::quat rotate = glm::slerp(rotate0, rotate0 * rotate1, mAffineBlendingTime);
			glm::normalize(rotate);
			glm::quat realRotation = pTr->GetRotationQuaterions();
			realRotation *= glm::inverse(prevRotation);
			pTr->SetRotationAngles(glm::degrees(glm::eulerAngles(rotate * realRotation)));
			prevRotation = rotate;
			prevNextRotation = rotate1;
			//glm::mat4 R = glm::toMat4(rotate);

			// Translate
			glm::vec3 translate0 = pCurrentAffineAnimation->InterpolateTranslate(mCurrentAffineTime);
			glm::vec3 translate1 = pNextAffineAnimation->InterpolateTranslate(mNextAffineTime);
			glm::vec3 translate = glm::lerp(translate0, translate0 + translate1, mAffineBlendingTime);
			glm::vec3 realTranslation = pTr->GetPosition();
			realTranslation -= prevTranslation;
			pTr->SetPosition(realTranslation + translate);
			prevTranslation = translate;
			prevNextTranslation = translate1;
			//glm::mat4 T = glm::translate(glm::mat4(1.0f), translate);

			//affineAnimationMatrix = T * R * S;
		}
		else {
			AffineAnimation* pCurrentAffineAnimation = mpCurrentAffineState->pAffineAnimation;

			// Scale
			glm::vec3 scale = pCurrentAffineAnimation->InterpolateScale(mCurrentAffineTime);
			glm::vec3 realScale = pTr->GetScale();
			realScale /= prevScale;
			pTr->SetScale(realScale * scale);
			prevScale = scale;
			//glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

			// Rotate
			glm::quat rotate = pCurrentAffineAnimation->InterpolateRotate(mCurrentAffineTime);
			glm::quat realRotation = pTr->GetRotationQuaterions();
			realRotation *= glm::inverse(prevRotation);
			glm::normalize(realRotation);
			pTr->SetRotationAngles(glm::degrees(glm::eulerAngles(glm::normalize(realRotation * rotate))));
			prevRotation = rotate;
			//glm::mat4 R = glm::toMat4(rotate);

			// Translate
			glm::vec3 translate = pCurrentAffineAnimation->InterpolateTranslate(mCurrentAffineTime);
			glm::vec3 realTranslation = pTr->GetPosition();
			realTranslation -= prevTranslation;
			pTr->SetPosition(realTranslation + translate);
			prevTranslation = translate;
			//glm::mat4 T = glm::translate(glm::mat4(1.0f), translate);

			//affineAnimationMatrix = T * R * S;
		}

		mAffineAnimationMatrix = affineAnimationMatrix;
		mComputeAffineAnimation = false;
	}

	return mAffineAnimationMatrix;
}

const char* Animator::GetSkellState()
{
	if(mpCurrentSkeletalState)
		return mpCurrentSkeletalState->stateName.c_str();

	return "None";
}

std::string Animator::GetAffineState()
{
	if (mpCurrentAffineState)
		return mpCurrentAffineState->stateName;

	return "None";
}

std::vector<std::string> Animator::GetAffineStates()
{
	std::vector<std::string> states;
	for (auto& element : mAffineStateMap)
		if (element.first != "None")
			states.push_back(element.first);

	return states;
}

std::vector<glm::mat4> Animator::GetSkeletalTransformations(Model* a_pModel)
{
	// Interpolate Between Keyframes of each joint to get Skeletal Transformations
	if (mComputeSkeletalAnimation) {
		// Clear list before refilling
		mSkeletalTransformations.clear();
		
		if (mpCurrentSkeletalState) {
			// Blend Animations
			if (mpNextSkeletalState)
				ComputeBlendedTransformations(a_pModel);
			else
				ComputeTransformations(a_pModel);

			int numBones = int(a_pModel->mpSkeleton->mBones.size());
			std::vector<Bone> bones = a_pModel->mpSkeleton->mBones;
			// Push the final bone transformations into the output transformation vector
			for (int i = 0; i < numBones; ++i)
				mSkeletalTransformations.push_back(bones[i].finalTransformation);
		}

		mComputeSkeletalAnimation = false;
	}

	return mSkeletalTransformations;
}

bool Animator::ChangeState(const std::string& a_state)
{
	return ChangeAffineState(a_state) || ChangeSkeletalState(a_state);
}

bool Animator::ChangeAffineState(const std::string& a_state)
{
	// Change the current animation state if the state exists and isn't the current state
	if (mAffineStateMap.find(a_state) != mAffineStateMap.end()) {
		if (mpCurrentAffineState) {
			if (a_state != mpCurrentAffineState->stateName) {
				mpRuntimeAffineState = mAffineStateMap[a_state];
				return true;
			}
		}
		else {
			mpRuntimeAffineState = mAffineStateMap[a_state];
			return true;
		}
	}
	else if (mDefaultAffineState != "None") {
		return ChangeState(mDefaultAffineState) && a_state == mDefaultAffineState;
	}
	else {
		// Reset Current Animation
		mpCurrentAffineState = nullptr;
		mCurrentAffineTime = 0.0f;

		// Reset Next Animation
		mpNextAffineState = nullptr;
		mNextAffineTime = 0.0f;

		// Reset Blending Time
		mAffineBlendingTime = 1.0f;
	}

	return false;
}

bool Animator::ChangeSkeletalState(const std::string& a_state)
{
	// Change the current animation state if the state exists and isn't the current state
	if (mSkeletalStateMap.find(a_state) != mSkeletalStateMap.end()) {
		if (mpCurrentSkeletalState) {
			if (a_state != mpCurrentSkeletalState->stateName) {
				mpRuntimeSkeletalState = mSkeletalStateMap[a_state];
				return true;
			}
			else {
				// Reset Next Animation
				mpNextSkeletalState = nullptr;
				mNextSkeletalTime = 0.0f;

				// Reset Blending Time
				mSkeletalBlendingTime = 1.0f;
			}
		}
		else {
			mpRuntimeSkeletalState = mSkeletalStateMap[a_state];
			return true;
		}
	}
	else if (mDefaultSkeletalState != "None") {
		return ChangeState(mDefaultSkeletalState) && a_state == mDefaultSkeletalState;
	}
	else {
		// Reset Current Animation
		mpCurrentSkeletalState = nullptr;
		mCurrentSkeletalTime = 0.0f;

		// Reset Next Animation
		mpNextSkeletalState = nullptr;
		mNextSkeletalTime = 0.0f;

		// Reset Blending Time
		mSkeletalBlendingTime = 1.0f;
	}

	return false;
}

GEAR_API void Animator::HandleEvent(Event * a_pEvent)
{
	
}

void Animator::UpdateAffineAnimation(float a_deltaTime)
{
	// Update current and next animations with runtime information
	if (!mpCurrentAffineState) {
		mpCurrentAffineState = mpRuntimeAffineState;
		mCurrentAffineTime = 0.0f;

		prevScale = glm::vec3(1.0f);
		prevRotation = glm::quat(glm::vec3(0.0f));
		prevTranslation = glm::vec3(0.0f);
	}
	else if (mpRuntimeAffineState) {
		if (mpRuntimeAffineState != mpNextAffineState) {
			mpNextAffineState = mpRuntimeAffineState;
			mNextAffineTime = 0.0f;

			prevNextScale = glm::vec3(1.0f);
			prevNextRotation = glm::quat(glm::vec3(0.0f));
			prevNextTranslation = glm::vec3(0.0f);

			mAffineBlendingTime = 0.0f;
		}
	}
	mpRuntimeAffineState = nullptr;

	// Update animatiom time only if it is not paused
	if (!mPause) {
		// Update Blending Time
		if (mAffineBlendingTime < 1.0f) {
			mAffineBlendingTime += a_deltaTime / mpNextAffineState->blendDuration;

			// If blending has ended
			if (mAffineBlendingTime >= 1.0f) {
				// Set current to next
				mpCurrentAffineState = mpNextAffineState;
				mCurrentAffineTime = mNextAffineTime;

				prevScale = prevNextScale;
				prevRotation = prevNextRotation;
				prevTranslation = prevNextTranslation;

				// Reset Next
				mpNextAffineState = nullptr;
				mNextAffineTime = 0.0f;
			}
			else {
				// Update Next Animation Time
				AffineAnimation* pNextAffineAnimation = mpNextAffineState->pAffineAnimation;
				float ticksPerSecond = pNextAffineAnimation->mTicksPerSecond;
				float duration = pNextAffineAnimation->mDuration;

				if (mNextAffineTime + a_deltaTime * ticksPerSecond < duration || mpNextAffineState->stateName == mDefaultAffineState) {
					mNextAffineTime += a_deltaTime * ticksPerSecond;
				}

				mNextAffineTime = fmod(mNextAffineTime, duration);
			}
		}

		// Update Animation Time
		if (mpCurrentAffineState) {
			AffineAnimation* pCurrentAffineAnimation = mpCurrentAffineState->pAffineAnimation;
			float ticksPerSecond = pCurrentAffineAnimation->mTicksPerSecond;
			float duration = pCurrentAffineAnimation->mDuration;

			// Increment animation time if the current animation time is 
			// less than the current animation's duration or if the current 
			// state is the default state
			if (mCurrentAffineTime >= duration) {
				prevScale = glm::vec3(1.0f);
				prevRotation = glm::quat(glm::vec3(0.0f));
				prevTranslation = glm::vec3(0.0f);
				mCurrentAffineTime = a_deltaTime * ticksPerSecond;

				// Return to the default state if you aren't already blending
				if (mpCurrentAffineState->stateName != mDefaultAffineState && mpCurrentAffineState->returnToDefault && !mpNextAffineState)
					mpCurrentAffineState = mAffineStateMap[mDefaultAffineState];
			}
			else {
				mCurrentAffineTime += a_deltaTime * ticksPerSecond;

				if (mCurrentAffineTime >= duration) {
					mCurrentAffineTime = duration;
				}
			}

			//mCurrentAffineTime = fmod(mCurrentAffineTime, duration);
		}
	}
}

void Animator::UpdateSkeletalAnimation(float a_deltaTime)
{
	// Update current and next animations with runtime information
	if (!mpCurrentSkeletalState) {
		mpCurrentSkeletalState = mpRuntimeSkeletalState;
		mCurrentSkeletalTime = 0.0f;
	}
	else if (mpRuntimeSkeletalState) {
		if (mpRuntimeSkeletalState != mpNextSkeletalState) {
			mpNextSkeletalState = mpRuntimeSkeletalState;
			mNextSkeletalTime = 0.0f;

			mSkeletalBlendingTime = 0.0f;
		}
	}
	mpRuntimeSkeletalState = nullptr;

	// Update animatiom time only if it is not paused
	if (!mPause) {
		// Update Blending Time
		if (mSkeletalBlendingTime < 1.0f) {
			mSkeletalBlendingTime += a_deltaTime / mpNextSkeletalState->blendDuration;

			// If blending has ended
			if (mSkeletalBlendingTime >= 1.0f) {
				// Set current to next
				mpCurrentSkeletalState = mpNextSkeletalState;
				mCurrentSkeletalTime = mNextSkeletalTime;

				// Reset Next
				mpNextSkeletalState = nullptr;
				mNextSkeletalTime = 0.0f;
			}
			else {
				// Update Next Animation Time
				SkeletalAnimation* pNextSkeletalAnimation = mpNextSkeletalState->pSkeletalAnimation;
				float ticksPerSecond = pNextSkeletalAnimation->mTicksPerSecond;
				float duration = pNextSkeletalAnimation->mDuration;

				if (mNextSkeletalTime + a_deltaTime * ticksPerSecond >= duration && mpNextSkeletalState->stateName != mDefaultSkeletalState) {
					if (mpNextSkeletalState->returnToDefault) {
						// Expect strange behavior frome this.  This says that if the animation you
						// are blending to has ended before the blend has completed, then blend to
						// the default state for the remaineder of the previously defined blend duration
						mpNextSkeletalState = mSkeletalStateMap[mDefaultSkeletalState];
						mNextSkeletalTime = 0.0f;
					}
				}
				else {
					mNextSkeletalTime += a_deltaTime * ticksPerSecond;
				}

				mNextSkeletalTime = fmod(mNextSkeletalTime, duration);
			}
		}

		// Update Animation Time
		if (mpCurrentSkeletalState) {
			SkeletalAnimation* pCurrentSkeletalAnimation = mpCurrentSkeletalState->pSkeletalAnimation;
			float ticksPerSecond = pCurrentSkeletalAnimation->mTicksPerSecond;
			float duration = pCurrentSkeletalAnimation->mDuration;

			// Increment animation time if the current animation time is 
			// less than the current animation's duration or if the current 
			// state is the default state
			if (mCurrentSkeletalTime + a_deltaTime * ticksPerSecond >= duration && mpCurrentSkeletalState->stateName != mDefaultSkeletalState) {
				// Return to the default state if you aren't already blending
				if (!mpNextSkeletalState && mpCurrentSkeletalState->returnToDefault)
					ChangeSkeletalState(mDefaultSkeletalState);
				mCurrentSkeletalTime = 0.0f;
			}
			else {
				mCurrentSkeletalTime += a_deltaTime * ticksPerSecond;
			}

			mCurrentSkeletalTime = fmod(mCurrentSkeletalTime, duration);
		}
	}
}

void Animator::ComputeTransformations(Model* a_pModel)
{
	SkeletalAnimation* pCurrentSkeletalAnimation = mpCurrentSkeletalState->pSkeletalAnimation;

	std::stack<Bone*> boneStack;
	boneStack.push(a_pModel->mpSkeleton->mpSkeletonRoot);

	// Compute the final transformation of each joint given the current animation time
	while (!boneStack.empty()) {
		Bone* pCurrentBone = boneStack.top();
		boneStack.pop();

		// Assumes that the current bone is non-null

		glm::mat4 boneTransformation = pCurrentBone->bindTransformation;

		if (pCurrentSkeletalAnimation->mJointScaleMap.find(pCurrentBone->name) != pCurrentSkeletalAnimation->mJointScaleMap.end()) {

			// Scale
			glm::vec3 scale = pCurrentSkeletalAnimation->InterpolateScale(pCurrentBone->name, mCurrentSkeletalTime);
			glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

			// Rotate
			glm::quat rotate = pCurrentSkeletalAnimation->InterpolateRotate(pCurrentBone->name, mCurrentSkeletalTime);
			glm::mat4 R = glm::toMat4(rotate);

			// Translate
			glm::vec3 translate = pCurrentSkeletalAnimation->InterpolateTranslate(pCurrentBone->name, mCurrentSkeletalTime);
			glm::mat4 T = glm::translate(glm::mat4(1.0f), translate / a_pModel->mMaxLength);

			boneTransformation = T * R * S;
		}

		pCurrentBone->worldTransformation = boneTransformation;
		if (pCurrentBone->pParent)
			pCurrentBone->worldTransformation = pCurrentBone->pParent->worldTransformation * boneTransformation;

		Skeleton* pSkeleton = a_pModel->mpSkeleton;
		unsigned int boneIndex = pSkeleton->mBoneMap[pCurrentBone->name];
		pSkeleton->mBones[boneIndex].finalTransformation = a_pModel->mGlobalInverse * pCurrentBone->worldTransformation * pSkeleton->mBones[boneIndex].offsetMatrix;

		// Push child bones onto stack for computation
		for (int i = 0; i < pCurrentBone->children.size(); ++i)
			boneStack.push(pCurrentBone->children[i]);
	}
}

void Animator::ComputeBlendedTransformations(Model* a_pModel)
{
	SkeletalAnimation* pCurrentSkeletalAnimation = mpCurrentSkeletalState->pSkeletalAnimation;
	SkeletalAnimation* pNextSkeletalAnimation = mpNextSkeletalState->pSkeletalAnimation;

	std::stack<Bone*> boneStack;
	boneStack.push(a_pModel->mpSkeleton->mpSkeletonRoot);

	// Compute the final transformation of each joint given the current animation time
	while (!boneStack.empty()) {
		Bone* pCurrentBone = boneStack.top();
		boneStack.pop();

		// Assumes that the current bone is non-null

		glm::mat4 boneTransformation = pCurrentBone->bindTransformation;

		// Blend
		if (pCurrentSkeletalAnimation->mJointScaleMap.find(pCurrentBone->name) != pCurrentSkeletalAnimation->mJointScaleMap.end()) {
			// Scale
			glm::vec3 scale0 = pCurrentSkeletalAnimation->InterpolateScale(pCurrentBone->name, mCurrentSkeletalTime);
			glm::vec3 scale1 = pNextSkeletalAnimation->InterpolateScale(pCurrentBone->name, mNextSkeletalTime);
			glm::vec3 scale = glm::lerp(scale0, scale1, mSkeletalBlendingTime);
			glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

			// Rotate
			glm::quat rotate0 = pCurrentSkeletalAnimation->InterpolateRotate(pCurrentBone->name, mCurrentSkeletalTime);
			glm::quat rotate1 = pNextSkeletalAnimation->InterpolateRotate(pCurrentBone->name, mNextSkeletalTime);
			glm::quat rotate = glm::slerp(rotate0, rotate1, mSkeletalBlendingTime);
			glm::normalize(rotate);
			glm::mat4 R = glm::toMat4(rotate);

			// Translate
			glm::vec3 translate0 = pCurrentSkeletalAnimation->InterpolateTranslate(pCurrentBone->name, mCurrentSkeletalTime);
			glm::vec3 translate1 = pNextSkeletalAnimation->InterpolateTranslate(pCurrentBone->name, mNextSkeletalTime);
			glm::vec3 translate = glm::lerp(translate0, translate1, mSkeletalBlendingTime);
			glm::mat4 T = glm::translate(glm::mat4(1.0f), translate / a_pModel->mMaxLength);

			boneTransformation = T * R * S;
		}

		pCurrentBone->worldTransformation = boneTransformation;
		if (pCurrentBone->pParent)
			pCurrentBone->worldTransformation = pCurrentBone->pParent->worldTransformation * boneTransformation;

		Skeleton* pSkeleton = a_pModel->mpSkeleton;
		unsigned int boneIndex = pSkeleton->mBoneMap[pCurrentBone->name];
		pSkeleton->mBones[boneIndex].finalTransformation = a_pModel->mGlobalInverse * pCurrentBone->worldTransformation * pSkeleton->mBones[boneIndex].offsetMatrix;

		// Push child bones onto stack for computation
		for (int i = 0; i < pCurrentBone->children.size(); ++i)
			boneStack.push(pCurrentBone->children[i]);
	}
}