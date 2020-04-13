#include "pch.h"
#include "Engine.h"
#include "Managers/Log.h"
#include "Core.h"
#include "UIComponent.h"
#include "Managers/EventManager.h"
#include "Events/UIActionEvent.h"
#include "Events/InitLevelEvent.h"
#include "Events/UpdateLevelEvent.h"
#include "Events/UpdateSliderPositionEvent.h"
#include "Managers/SceneManager.h"
#include "Managers/ObjectFactory.h"
#include "Managers/ResourceManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/InputManager.h"
#include "Managers/CameraManager.h"
#include "Managers/ComponentManager.h"
#include "Managers/AudioManager.h"
#include "Managers/AudioManager.h"
#include "Managers/Renderer.h"
#include "Resources/JsonData.h"
#include "Utility.h"

#include "Managers/Log.h"

UIComponent::UIComponent() : Component("UIComponent")
{
	mTimer = 0.0f;
	mInitialTime = 0.0f;
	mScaleFactor = 1.0f;
	mSliderPosition = glm::vec3(0);
	mIsLevelButton = false;
	EventManager::GetInstance()->Subscribe("UIActionEvent", GetOwnerID());
	//EventManager::GetInstance()->Subscribe("LevelComleteEvent", GetOwnerID());

}

UIComponent::~UIComponent()
{

}

UIComponent* UIComponent::Create()
{
	return new UIComponent();
}

void UIComponent::Init()
{
	if (mData.sRadioButtonLabel == "Mute")
	{
		bool temp = AudioManager::GetInstance()->GetMute();
		if (AudioManager::GetInstance()->GetCurrentVolume() > 0.0f && !temp)
		{
			mData.sRadioButtonSelected = 1;
		}
		else
		{
			mData.sRadioButtonSelected = 0;
		}
	}

	if (mData.sRadioButtonLabel == "FullScreen")
	{
		if (!Engine::GetInstance()->mFullScreen)
		{
			mData.sRadioButtonSelected = 1;
		}
		else
		{
			mData.sRadioButtonSelected = 0;
		}
	}
	if (mData.sRadioButtonLabel == "Resolution")
	{
		for (int i = 0; i < mData.sRadioButtonOptions.size(); ++i)
		{
			std::vector<std::string> tokens;
			std::istringstream f(mData.sRadioButtonOptions[i]); std::string s;
			while (getline(f, s, 'x')) { tokens.push_back(s); }
			int width = std::stoi(tokens[0]);
			int height = std::stoi(tokens[1]);
			if (Engine::GetInstance()->mWindowWidth == width && Engine::GetInstance()->mWindowHeight == height)
				mData.sRadioButtonSelected = i;
		}
	}
	mData.sSliderValue = (int)(AudioManager::GetInstance()->GetCurrentVolume() * 100.0f);

	std::vector<LevelData> *levelData = SCENE_MANAGER->GetLevelData();

	for (LevelData &element : *levelData)
	{
		if (element.LevelName.compare(mData.sButtonAction) == 0)
		{
			mIsLevelButton = true;
			mHighScore = element.highScore;
			mLevelNumber = element.LevelNumber;
			mUnlockScore = element.scoreToUnlock;
			mIsUnlocked = element.isUnlocked;
			mLevelName = element.LevelName;
		}
	}

	originalLabel = mData.sButtonLabel;

}

void UIComponent::Reset()
{

}

void UIComponent::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["Type"];
	if (!dataRoot.isNull())
	{
		if (dataRoot.asString() == "Image")
		{
			mData.sType = IMAGE;
			dataRoot = a_root["Description"];
			if (!dataRoot.isNull())
			{
				mData.sImageDescription = dataRoot.asString();
			}
		}
		else if (dataRoot.asString() == "Button")
		{
			mData.sType = BUTTON;
			dataRoot = a_root["Label"];
			if (!dataRoot.isNull())
			{
				mData.sButtonLabel = dataRoot.asString();
			}
			dataRoot = a_root["Action"];
			if (!dataRoot.isNull())
			{
				mData.sButtonAction = dataRoot.asString();
			}
		}
		else if (dataRoot.asString() == "Radio")
		{
			mData.sType = RADIOBUTTON;
			dataRoot = a_root["Label"];
			if (!dataRoot.isNull())
			{
				mData.sRadioButtonLabel = dataRoot.asString();
			}
			dataRoot = a_root["Options"];
			for (Json::Value::iterator itr = dataRoot.begin(); itr != dataRoot.end(); ++itr)
			{
				if (!itr->isNull())
				{
					mData.sRadioButtonOptions.push_back(itr->asString());
				}
			}
		}
		else if (dataRoot.asString() == "Slider")
		{
			mData.sType = SLIDER;
			dataRoot = a_root["Label"];
			if (!dataRoot.isNull())
			{
				mData.sSliderLabel = dataRoot.asString();
			}
			dataRoot = a_root["Min Value"];
			if (!dataRoot.isNull())
			{
				mData.sMinNum = dataRoot.asInt();
			}
			dataRoot = a_root["Max Value"];
			if (!dataRoot.isNull())
			{
				mData.sMaxNum = dataRoot.asInt();
			}
		}
		else if (dataRoot.asString() == "Text")
		{
			mData.sType = TEXT;
			dataRoot = a_root["String"];
			if (!dataRoot.isNull())
			{
				mData.sTextString = dataRoot.asString();
			}
		}
		else
		{
			mData.sType = INVALID;
		}
	}
}

void UIComponent::HandleEvent(Event* a_pEvent)
{
	if (a_pEvent->GetType() == "UIActionEvent")
	{
		UIActionEvent* pUIEvent = static_cast<UIActionEvent*>(a_pEvent);

		//Special case- when mute and press on volume slider
		if (pUIEvent->mDescription == "Volume" && mData.sRadioButtonLabel == "Mute")
		{
			Init();
		}

		if (pUIEvent->mOwnerId == GetOwnerID())
		{
			if (!mData.sButtonAction.empty())
			{
				std::string loadFile = mData.sButtonAction;
				if (pUIEvent->mDescription == "Restart")
				{
					SCENE_MANAGER->LoadActionScene("RestartConfirmation");
				}
				if (pUIEvent->mDescription == "Main Menu")
				{
					SCENE_MANAGER->LoadActionScene("MainMenuConfirmation");
				}
				if (pUIEvent->mDescription == "Yes" && mData.sButtonAction == "MainMenu")
				{
					SCENE_MANAGER->ClearAll();
				}
				if (pUIEvent->mDescription == "Yes" && mData.sButtonAction == "Restart")
				{
					SCENE_MANAGER->ClearLevel(SCENE_MANAGER->GetCurrent());
					SCENE_MANAGER->ClearLevel(SCENE_MANAGER->GetCurrent());
					loadFile = SCENE_MANAGER->GetCurrent();
					SCENE_MANAGER->ClearLevel(loadFile);
					loadFile.erase(loadFile.size() - 5, 5);
				}
				std::vector<std::string> levelNames = GetUniqueDirectoryFiles(".\\Resources\\LevelData\\");
				if (std::find(levelNames.begin(), levelNames.end(), loadFile) != levelNames.end()) {
					if (originalLabel == mData.sButtonLabel) {
						SCENE_MANAGER->ClearAll();
						SetupLevel(loadFile);
					}
				}
				else {		
					if(mData.sButtonAction == "LevelScreenFinal")
						SCENE_MANAGER->ClearAll();
					SCENE_MANAGER->PushScene(loadFile);
					ObjectFactory::GetInstance()->LoadLevel(loadFile);
					ENTITY_MANAGER->ClearRuntimeLists();
					COMPONENT_MANAGER->ClearRuntimeLists();
				}
			}
			else
			{
				if (pUIEvent->mDescription == "Yes")
				{
					Engine::GetInstance()->SetIsRunning(false);
				}
				else if (pUIEvent->mDescription == "Volume")
				{
					AudioManager::GetInstance()->SetAllVolume((float)mData.sSliderValue / 100);
				}
				else if (pUIEvent->mDescription == "Mute")
				{
					if (pUIEvent->mData == "On") {

						AudioManager::GetInstance()->SetAllVolume(0.0);
						AudioManager::GetInstance()->SetMute(true);
					}
					else
					{
						AudioManager::GetInstance()->SetAllVolume(AudioManager::GetInstance()->GetCurrentVolume());
						AudioManager::GetInstance()->SetMute(false);
					}
				}
				else if (pUIEvent->mDescription == "FullScreen")
				{
					if (pUIEvent->mData == "On") {
						Engine::GetInstance()->mFullScreen = true;
						Engine::GetInstance()->FlipScreenMode();
					}
					else {

						Engine::GetInstance()->mFullScreen = false;
						Engine::GetInstance()->FlipScreenMode();
					}
				}
				else if (pUIEvent->mDescription == "Resolution")
				{
					std::vector<std::string> tokens;
					std::istringstream f(pUIEvent->mData); std::string s;
					while (getline(f, s, 'x')) { tokens.push_back(s); }
					int width = std::stoi(tokens[0]);
					int height = std::stoi(tokens[1]);
					Engine::GetInstance()->FlipScreenMode(width,height);
				}
				else
				{
					std::string current = SCENE_MANAGER->GetCurrent();
					SCENE_MANAGER->ClearLevel(current);
					if (current == "PauseMenu") {
						PauseEvent* pPauseEvent = static_cast<PauseEvent*>(MEMORY_MANAGER->GetEventFromMemory("PauseEvent"));
						pPauseEvent->mPause = false;
						EVENT_MANAGER->BroadcastEvent(pPauseEvent);
					}
				}
			}
		}
	}
	if (a_pEvent->GetType() == "UpdateLevelEvent")
	{
		UpdateLevelEvent* pUpdateLevelEvent = static_cast<UpdateLevelEvent*>(a_pEvent);
		mTimer = pUpdateLevelEvent->mCurrentTime;
	}
	if (a_pEvent->GetType() == "InitLevelEvent")
	{
		InitLevelEvent* pInitLevelEvent = static_cast<InitLevelEvent*>(a_pEvent);
		mInitialTime = pInitLevelEvent->mTime;
		mTimer = 0.0f;
	}
	if (mData.sImageDescription == "Slider_Image" && a_pEvent->GetType() == "UpdateSliderPositionEvent")
	{
		Transform* pTr = ComponentManager::GetInstance()->GetComponent<Transform>(GetOwnerID(), "Transform");
		UpdateSliderPositionEvent* pSliderPosEvent = static_cast<UpdateSliderPositionEvent*>(a_pEvent);
		pTr->SetPosition(pSliderPosEvent->mSliderPosition);
	}

	if (a_pEvent->GetType() == "LevelCompleteEvent")
	{
		LevelCompleteEvent* pLCEvent = static_cast<LevelCompleteEvent*>(a_pEvent);

		std::string temp = SCENE_MANAGER->GetCurrent();
		int length = temp.length();
		std::string CompletedLevel = temp.substr(0, length - 5);

		std::vector<LevelData> *levelData = SCENE_MANAGER->GetLevelData();

		for (LevelData &element : *levelData)
		{

			if (element.LevelName.compare(CompletedLevel) == 0)
			{
				if (element.highScore < pLCEvent->mScore) {
					element.highScore = pLCEvent->mScore;
				}
			}
		}
// 		if (CompletedLevel == mLevelName)
// 		{
// 			mHighScore = pLCEvent->mScore;
// 		}
	}

}

void UIComponent::Clone(int id)
{
	UIComponent *rhs = static_cast<UIComponent*>(COMPONENT_MANAGER->GetComponent<UIComponent>(id, "UIComponent"));
	mData = rhs->mData;
}

void UIComponent::Update(float a_deltaTime)
{
	if ((mData.sImageDescription == "Slider" || mData.sImageDescription == "Slider_Image") && mTimer >= 0.0f)
	{
		mScaleFactor = mTimer / mInitialTime;
		mScaleFactor = 1.0f - mScaleFactor;

		if (mData.sImageDescription == "Slider")
		{
			UpdateSliderPositionEvent* pSliderPosEvent = static_cast<UpdateSliderPositionEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("UpdateSliderPositionEvent"));
			Transform* pTr = ComponentManager::GetInstance()->GetComponent<Transform>(GetOwnerID(), "Transform");
			glm::vec3 position = pTr->GetPosition();
			glm::vec3 scale = pTr->GetScale();
			scale.x *= mScaleFactor;
			position.x += scale.x;
			pSliderPosEvent->mSliderPosition = position;
			EVENT_MANAGER->BroadcastEvent(pSliderPosEvent);
		}
	}

	std::vector<LevelData> *levelData = SCENE_MANAGER->GetLevelData();
	
	for (LevelData &element : *levelData)
	{
		if (mLevelNumber == element.LevelNumber + 1)
		{
			if (element.highScore >= mUnlockScore) {
				mIsUnlocked = true;
				originalLabel = mData.sButtonLabel;
			}
		}
		if (element.LevelName.compare(mData.sButtonAction) == 0)
		{			
			element.isUnlocked = mIsUnlocked;
			if (element.isUnlocked == false)
			{
				tempLabel = "Score " + to_string(mUnlockScore) + " on Level " + to_string(element.LevelNumber - 1 );
				mData.sButtonLabel = tempLabel;
			}
		}
	}

	if (mData.sRadioButtonLabel == "FullScreen")
	{
		if (!Engine::GetInstance()->mFullScreen)
		{
			mData.sRadioButtonSelected = 1;
		}
		else
		{
			mData.sRadioButtonSelected = 0;
		}
	}
}

void UIComponent::SetupLevel(std::string a_levelName)
{
	// Clear all old data from the previous scene so it can be reused
	ENTITY_MANAGER->ClearRuntimeLists();
	COMPONENT_MANAGER->ClearRuntimeLists();

	RENDERER->Fade(true, 1.0f);

	// Creates and loads entities and their components from specified level file
	Json::Value root = RESOURCE_MANAGER->LoadJsonData(a_levelName, ".\\Resources\\LevelData\\")->mRoot;
	InitLevelEvent* initLevelEvent = static_cast<InitLevelEvent*>(MEMORY_MANAGER->GetEventFromMemory("InitLevelEvent"));

	// Load Level
	Json::Value levelData = root["LevelFile"];
	if (!levelData.isNull()) {
		std::string levelFile = levelData.asString();
		SCENE_MANAGER->PushScene(levelFile);
		ObjectFactory::GetInstance()->LoadLevel(levelFile);
		ENTITY_MANAGER->ClearRuntimeLists();
		COMPONENT_MANAGER->ClearRuntimeLists();
	}

	levelData = root["Name"];
	if (!levelData.isNull())
		initLevelEvent->mLevelName = levelData.asString();

	levelData = root["Music"];
	if (!levelData.isNull())
		initLevelEvent->mMusic = levelData.asString();

	levelData = root["Time"];
	if (!levelData.isNull())
		initLevelEvent->mTime = levelData.asFloat();

	levelData = root["Difficulty"];
	if (!levelData.isNull())
		for (unsigned int i = 0; i < levelData.size(); ++i)
			initLevelEvent->mGames.push_back(levelData[i].asInt());

	// Get the number of players
	int numPlayers = 2;
	int numControllers = int(INPUT_MANAGER->GetPlayerGamePads().size());
	if (numControllers > 2)
		numPlayers = numControllers;
	
	levelData = root["Level Number"];
	if (!levelData.isNull())
		initLevelEvent->mLevelNumber = levelData.asInt();

	levelData = root["Grading"];
	if (!levelData.isNull()) {
		Json::Value grades = levelData[numPlayers - 2];
		for (unsigned int i = 0; i < grades.size(); ++i)
			initLevelEvent->mGrading.push_back(grades[i].asFloat());

	}

	initLevelEvent->mNumPlayers = numPlayers;

	EVENT_MANAGER->BroadcastEvent(initLevelEvent);
}