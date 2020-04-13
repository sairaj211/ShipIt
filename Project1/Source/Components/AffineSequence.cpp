#include "pch.h"
#include "Managers/Log.h"

#include "Core.h"

#include "AffineSequence.h"
#include "Animator.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/EventManager.h"
#include "Managers/MemoryManager.h"
#include "Resources/JsonData.h"
#include "Events/ChangeAnimationStateEvent.h"
#include "Events/PauseEvent.h"
#include "Entity.h"

AffineSequence::AffineSequence() : Component("AffineSequence")
{
	mIndex = 0;
	mTimerStart = 0.0f;
	mTimer = mTimerStart;
	mMode = PlayMode::COMMAND;

	mPauseTimer = false;
	mModeStrings.push_back("Command");
	mModeStrings.push_back("Start Duration");
	mModeStrings.push_back("End Duration");
	mModeStrings.push_back("End Animation");
}

AffineSequence::~AffineSequence()
{

}

void AffineSequence::Reset()
{
	mSequence.clear();
	mIndex = 0;
	mTimerStart = 0.0f;
	mTimer = mTimerStart;
	mMode = PlayMode::COMMAND;

	mPauseTimer = false;
	mModeStrings.clear();

	Component::Reset();
}

AffineSequence* AffineSequence::Create()
{
	return new AffineSequence();
}

void AffineSequence::Update(float a_deltaTime)
{
	if (mSequence.size() == 0)
		return;

	if ((mMode == PlayMode::START_DURATION || mMode == PlayMode::END_DURATION) && !mPauseTimer) {
		if (fabsf(mTimer) <= 0.001f && mMode == PlayMode::START_DURATION) {
			Play();
		}

		mTimer += a_deltaTime / 1000.0f;

		if (mTimer >= mSequence[mIndex].second) {
			mTimer = 0.0f;

			if (mMode == PlayMode::END_DURATION)
				Play();
			Step();
		}
	}
	else if (mMode == PlayMode::END_ANIMATION) {
		Animator* pAnimator = COMPONENT_MANAGER->GetComponent<Animator>(mOwnerID, "Animator");
		if (pAnimator && pAnimator->GetAffineState() == "None") {
			Step();
			Play();
		}
	}
}

void AffineSequence::DisplayDebug()
{
	static int selectedIndex = 0;
	static bool swap = false;
	static bool add = false;
	if (ImGui::ListBoxHeader("Sequence")) {
		for (int i = 0; i < mSequence.size(); ++i) {
			bool selected = selectedIndex == i;
			if (ImGui::Selectable(mSequence[i].first.c_str(), selected)) {
				if (swap) {
					std::swap(mSequence[i], mSequence[selectedIndex]);
					swap = false;
				}
				else
					selectedIndex = i;
			}
		}

		ImGui::ListBoxFooter();
	}

	if (selectedIndex >= 0 && selectedIndex < mSequence.size())
		ImGui::DragFloat("Duration", &mSequence[selectedIndex].second);

	if (ImGui::Button("Add"))
		add = true;

	ImGui::SameLine();

	if (ImGui::Button("Remove") && (selectedIndex >= 0 && selectedIndex < mSequence.size())) {
		mSequence.erase(find(mSequence.begin(), mSequence.end(), mSequence[selectedIndex]));
		mIndex = 0;
		mTimer = mTimerStart;
	}

	ImGui::SameLine();

	if (ImGui::Button("Swap"))
		swap = true;

	if (add) {
		std::vector<std::string> affineStates;
		Animator* pAnimator = COMPONENT_MANAGER->GetComponent<Animator>(mOwnerID, "Animator", true);
		if (pAnimator)
			affineStates = pAnimator->GetAffineStates();

		if (ImGui::BeginCombo("Select New##SelectNew", "")) {
			for (int i = 0; i < affineStates.size(); ++i) {
				if (ImGui::Selectable(affineStates[i].c_str(), false)) {
					std::pair<std::string, float> animationPair;
					animationPair.first = affineStates[i];
					animationPair.second = 10.0f;
					mSequence.push_back(animationPair);
					add = false;
				}
			}
			ImGui::EndCombo();
		}
	}

	if (ImGui::BeginCombo("Mode##Mode", mModeStrings[mMode].c_str())) {
		for (int i = 0; i < mModeStrings.size(); ++i) {
			bool selected = mMode == i;
			if (ImGui::Selectable(mModeStrings[i].c_str(), selected)) {
				mMode = PlayMode(i);
				mTimer = mTimerStart;
			}
			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::Button("Resume"))
		ResumeTimer();

	ImGui::SameLine();

	if (ImGui::Button("Pause"))
		PauseTimer();

	ImGui::SameLine();

	if (ImGui::Button("Reset"))
		ResetTimer();

	if (ImGui::Button("Resume All")) {
		std::vector<Component*> sequenceComponents = COMPONENT_MANAGER->GetComponents("AffineSequence", true);
		for (int i = 0; i < sequenceComponents.size(); ++i)
			static_cast<AffineSequence*>(sequenceComponents[i])->ResumeTimer();
	}

	ImGui::SameLine();

	if (ImGui::Button("Pause All")) {
		std::vector<Component*> sequenceComponents = COMPONENT_MANAGER->GetComponents("AffineSequence", true);
		for (int i = 0; i < sequenceComponents.size(); ++i)
			static_cast<AffineSequence*>(sequenceComponents[i])->PauseTimer();
	}

	ImGui::SameLine();

	if (ImGui::Button("Reset All")) {
		std::vector<Component*> sequenceComponents = COMPONENT_MANAGER->GetComponents("AffineSequence", true);
		for (int i = 0; i < sequenceComponents.size(); ++i)
			static_cast<AffineSequence*>(sequenceComponents[i])->ResetTimer();
	}

	ImGui::DragFloat("Timer Start", &mTimerStart);

	char buffer[256];
	snprintf(buffer, 256, "Timer: %.2f\nCurrent Index: %d", mTimer, mIndex);
	ImGui::Text(buffer);

	if (ImGui::Button("Play"))
		PlayCurrent();
}

void AffineSequence::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["Sequence"];
	if (!dataRoot.isNull()) {
		mSequence.clear();
		for (unsigned int i = 0; i < dataRoot.size(); ++i) {
			Json::Value animation = dataRoot[i];
			std::pair<std::string, float> animationPair;

			Json::Value animationData = animation["Name"];
			if (!animationData.isNull())
				animationPair.first = animationData.asString();
			else
				animationPair.first = "None";

			animationData = animation["Duration"];
			if (!animationData.isNull())
				animationPair.second = animationData.asFloat();
			else
				animationPair.second = 1.0f;

			mSequence.push_back(animationPair);
		}
	}

	dataRoot = a_root["Mode"];
	if (!dataRoot.isNull())
		mMode = PlayMode(dataRoot.asInt());

	dataRoot = a_root["TimerStart"];
	if (!dataRoot.isNull())
		mTimer = mTimerStart = dataRoot.asFloat();
}

void AffineSequence::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	// Get the json root of the default owner entity
	std::string dataFile = ENTITY_MANAGER->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = RESOURCE_MANAGER->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["AffineSequence"];

	bool isDifferent = defaultRoot["Sequence"].size() != mSequence.size();
	if (!isDifferent) {
		for (unsigned int i = 0; i < defaultRoot["Sequence"].size(); ++i) {
			if (mSequence[i].first != defaultRoot["Sequence"][i]["Name"].asString() ||
				mSequence[i].second != defaultRoot["Sequence"][i]["Duration"].asFloat()) {
				isDifferent = true;
				break;
			}
		}
	}

	if (isDifferent || a_ignoreDefault) {
		Json::Value sequence(Json::arrayValue);
		for (int i = 0; i < mSequence.size(); ++i) {
			Json::Value animation;

			animation["Name"] = mSequence[i].first;
			animation["Duration"] = mSequence[i].second;

			sequence.append(animation);
		}
		a_root["Sequence"] = sequence;
	}

	if (mMode != PlayMode(defaultRoot["CompareFunction"].asInt()) || a_ignoreDefault)
		a_root["Mode"] = mMode;

	if (fabsf(defaultRoot["TimerStart"].asFloat() - mTimerStart) > 0.001f || a_ignoreDefault)
		a_root["TimerStart"] = mTimerStart;
}

void AffineSequence::HandleEvent(Event* a_pEvent)
{
	if (a_pEvent->GetType() == "PauseEvent") {
		mPaused = static_cast<PauseEvent*>(a_pEvent)->mPause;
		mPauseTimer = mPaused;
	}
}

void AffineSequence::PlayCurrent()
{
	if (mMode == PlayMode::COMMAND) {
		Play();
		Step();
	}
}

float AffineSequence::GetDuration()
{
	if (mIndex >= 0 && mIndex < mSequence.size()) {
		return mSequence[mIndex].second;
	}
	return 0.0f;
}

float AffineSequence::GetTimer()
{
	return mTimer;
}

std::string AffineSequence::GetAnimation()
{
	if (mIndex >= 0 && mIndex < mSequence.size()) {
		return mSequence[mIndex].first;
	}
	return "None";
}

void AffineSequence::ResumeTimer()
{
	mPauseTimer = false;
}

void AffineSequence::PauseTimer()
{
	mPauseTimer = true;
}

void AffineSequence::ResetTimer()
{
	mTimer = mTimerStart;
}

void AffineSequence::Play()
{
	Animator* pAnimator = COMPONENT_MANAGER->GetComponent<Animator>(mOwnerID, "Animator", true);
	if (!pAnimator)
		pAnimator = COMPONENT_MANAGER->GetRuntimeComponent<Animator>(mOwnerID, "Animator");
	if (pAnimator && mIndex < mSequence.size()) {
		if (pAnimator->ChangeAffineState(mSequence[mIndex].first)) {
			ChangeAnimationStateEvent* pChangeAnimationStateEvent = static_cast<ChangeAnimationStateEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("ChangeAnimationStateEvent"));
			pChangeAnimationStateEvent->mState = mSequence[mIndex].first;
			pChangeAnimationStateEvent->mID = mOwnerID;
			EventManager::GetInstance()->SendEvent(pChangeAnimationStateEvent, mOwnerID);
		}
	}
}

void AffineSequence::Step()
{
	if (mSequence.size() > 0)
		if (++mIndex >= mSequence.size())
			mIndex = 0;
}

void AffineSequence::Clone(int objID)
{
	
}