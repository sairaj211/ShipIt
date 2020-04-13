#include "pch.h"
#include "../Engine.h"
#include "../Entity.h"
#include "Utility.h"
#include "Core.h"
#include "Wall.h"
#include "Audio.h"
#include "LevelButton.h"
#include "Collider.h"
#include "Transform.h"
#include "ModelComponent.h"
#include "LocalLightComponent.h"
#include "Player.h"
#include "Controller.h"
#include "ImGui/imgui_stdlib.h"
#include "Resources/JsonData.h"
#include "Managers/ComponentManager.h"
#include "Managers/Log.h"
#include "Managers/EventManager.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/InputManager.h"
#include "Managers/CameraManager.h"
#include "Managers/ObjectFactory.h"
#include "Managers/EnemyStateManager.h"
#include "Managers/Renderer.h"
#include "Managers/SceneManager.h"
#include "Physics/PhysicsManager.h"
#include "Editor/LevelEditor.h"
#include "Events/ContactEvent.h"
#include "Events/LevelCompleteEvent.h"
#include "Events/UIActionEvent.h"
#include "Events/InitLevelEvent.h"
#include "Events/UpdateLevelEvent.h"

LevelButton::LevelButton() : Component("LevelButton")
{
	mChildID = -1;
	mOnce = true;
	mChildLight = nullptr;
	mScale = nullptr;
	in = false;
	tempID = -1;

}

LevelButton::~LevelButton()
{
}

void LevelButton::Init()
{

	std::vector<LevelData> *levelData = SCENE_MANAGER->GetLevelData();

	for (LevelData &element : *levelData)
	{
		if (element.LevelNumber == mLevelID)
		{
			mHighScore = element.highScore;
			mUnlockScore = element.scoreToUnlock;
			mIsUnlocked = element.isUnlocked;
			mLevelName = element.LevelName;
			mGrade = element.grade;
			
			if (mIsUnlocked && mWorldID > SCENE_MANAGER->GetCompletedWorld())
			{
				SCENE_MANAGER->SetCompletedWorld(mWorldID);
			}
		}
	}

}

void LevelButton::Reset()
{

	mChildID = -1;
	mOnce = true;
	mChildLight = nullptr;
	mScale = nullptr;
	in = false;
	tempID = -1;
	Component::Reset();
}

LevelButton * LevelButton::Create()
{
	return new LevelButton();
}

void LevelButton::Update(float a_deltaTime)
{
	std::vector<LevelData> *levelData = SCENE_MANAGER->GetLevelData();

	for (LevelData &element : *levelData)
	{
		if (mLevelID == element.LevelNumber + 1)
		{
			if (element.highScore >= mUnlockScore) {
				mIsUnlocked = true;
				
			}
		}
		if (element.LevelNumber == mLevelID)
		{
			element.isUnlocked = mIsUnlocked;
			if (element.isUnlocked && element.grade == 'X'){
				element.grade = 'D';
				mGrade = element.grade;
			}
		}
	}

	if (mOnce)
	{
		std::vector<int> lightID = ENTITY_MANAGER->GetEntity(mOwnerID)->GetChildIDs("Light");
		mChildID = lightID[0];

		std::vector<int> ScaleID = ENTITY_MANAGER->GetEntity(mOwnerID)->GetChildIDs("Scale");
		mScaleID = ScaleID[0];

		mChildLight = COMPONENT_MANAGER->GetComponent<LocalLightComponent>(mChildID, "LocalLightComponent");
		if (!mChildLight)
			mChildLight = COMPONENT_MANAGER->GetRuntimeComponent<LocalLightComponent>(mChildID, "LocalLightComponent");

		mScale = ENTITY_MANAGER->GetEntity(ScaleID[0]);

		mTransScale = static_cast<Transform*>(COMPONENT_MANAGER->GetComponent<Transform>(mScaleID, "Transform", true));
		glm::vec3 posScale = mTransScale->GetPosition();

		Transform* mTrans = static_cast<Transform*>(COMPONENT_MANAGER->GetComponent<Transform>(this->GetOwnerID(), "Transform", true));
		glm::vec3 pos = mTrans->GetPosition();
		mTransScale->SetScale(glm::vec3(3.0f, 5.0f, 3.0f));
		
		Transform* mPointLight = static_cast<Transform*>(COMPONENT_MANAGER->GetComponent<Transform>(mChildID, "Transform", true));
		glm::vec3 spoPos = mPointLight->GetPosition();
		spoPos.y = 8.376f;
		mPointLight->SetPosition(spoPos);

		posScale.x = pos.x;
		posScale.y = pos.y + 2.0f;
		posScale.z = pos.z;

		mTransScale->SetPosition(posScale);


		if (mGrade != 'X') {
			std::unordered_map<char, glm::vec3> data = SCENE_MANAGER->GetColorData();
			auto it = data.find(mGrade);
			glm::vec3 color = it->second;
			
			mChildLight->SetColor(color);
			ENTITY_MANAGER->EntityEnable(mScaleID, true);

			ModelComponent* mModelSpotLight = static_cast<ModelComponent*>(COMPONENT_MANAGER->GetComponent<ModelComponent>(mScaleID, "ModelComponent", true));
			mModelSpotLight->mpMaterial = RESOURCE_MANAGER->LoadMaterial(std::string(1, it->first));
			mChildLight->mIntensity = 15.0f;
		}
		
		
		if (mIsUnlocked) 
			ENTITY_MANAGER->EntityEnable(mChildID, true);
		else {
			ENTITY_MANAGER->EntityEnable(mChildID, false);
			ENTITY_MANAGER->EntityEnable(mScaleID, false);
		}

		mOnce = false;
	}



}

void LevelButton::DisplayDebug()
{
	ImGui::InputInt("Level ID : ", &mLevelID);
	ImGui::InputInt("World ID : ", &mWorldID);

}

void LevelButton::Serialize(Json::Value a_root)
{
	Json::Value dataRoot = a_root["LevelID"];
	if (!dataRoot.isNull())
		mLevelID = dataRoot.asInt();

	dataRoot = a_root["WorldID"];
	if (!dataRoot.isNull())
		mWorldID = dataRoot.asInt();


}

void LevelButton::DeSerialize(Json::Value & a_root, bool a_ignoreDefault)
{

	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	defaultRoot = defaultRoot["LevelButton"];

	if (mLevelID != defaultRoot["LevelID"].asInt() || a_ignoreDefault)
		a_root["LevelID"] = mLevelID;

	if (mWorldID != defaultRoot["WorldID"].asInt() || a_ignoreDefault)
		a_root["WorldID"] = mWorldID;
}

void LevelButton::HandleEvent(Event * a_pEvent)
{

	if (a_pEvent->GetType() == "ContactEvent") {
		ContactEvent* pContactEvent = static_cast<ContactEvent*>(a_pEvent);
		//LOG_INFO("YES");

		int otherID = -1;
		if (mOwnerID == pContactEvent->mId1)
			otherID = pContactEvent->mId2;
		if (mOwnerID == pContactEvent->mId2)
			otherID = pContactEvent->mId1;
		if (otherID == -1)
			return;

		/*if (pContactEvent->mContactType == OnTriggerExit) {
			if (tempID != -1)
				ENTITY_MANAGER->DeleteEntity(tempID);
		}*/

		if (otherID != -1 && pContactEvent->mContactType == OnTriggerStay) {
			//mChild->mIsOn = !mChild->mIsOn;
			std::vector<std::string> levelNames = GetUniqueDirectoryFiles(".\\Resources\\LevelData\\");
			
			if (std::find(levelNames.begin(), levelNames.end(), mLevelName) != levelNames.end()) {
		
				std::vector<Component*> mPlayer = COMPONENT_MANAGER->GetComponents("Player", true);
				Controller *mPlayerControl = static_cast<Controller*>(COMPONENT_MANAGER->GetComponent<Controller>(mPlayer[0]->GetOwnerID(), "Controller", true));

				if (mPlayerControl->IsTriggered("Interact")||INPUT_MANAGER->IsReleased(SDL_SCANCODE_SPACE)) {
					if (mIsUnlocked) {
						SCENE_MANAGER->ClearAll();
						SetupLevel(mLevelName);
						SCENE_MANAGER->SetWorldID(mWorldID);
					}
				}
			}

		}
	}
}

void LevelButton::Clone(int objID)
{
	
}




void LevelButton::SetupLevel(std::string a_levelName)
{
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