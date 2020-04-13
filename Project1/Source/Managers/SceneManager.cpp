#include "pch.h"
#include "SceneManager.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "ObjectFactory.h"
#include "ResourceManager.h"
#include "MemoryManager.h"
#include "InputManager.h"
#include "EventManager.h"
#include "Engine.h"
#include "Components/UIComponent.h"
#include "Resources/JsonData.h"
#include "Managers/Log.h"
#include "Resources/Text.h"
#include "Managers/InputManager.h"

SceneManager* SceneManager::mInstance = nullptr;

void SceneManager::Update()
{
	if (InputManager::GetInstance()->IsTriggered("start"))
	{
		std::string current = GetCurrent();
		if (current != "MainMenu" &&
			current != "Options" &&
			current != "LevelSelection" &&
			current != "Quit" &&
			current != "PauseMenu") {
			PauseEvent* pPauseEvent = static_cast<PauseEvent*>(MEMORY_MANAGER->GetEventFromMemory("PauseEvent"));
			pPauseEvent->mPause = true;
			EVENT_MANAGER->BroadcastEvent(pPauseEvent);
			PushScene("PauseMenu");
			ObjectFactory::GetInstance()->LoadLevel("PauseMenu");
		}
		else if (current == "PauseMenu") {
			SCENE_MANAGER->ClearLevel(current);
			PauseEvent* pPauseEvent = static_cast<PauseEvent*>(MEMORY_MANAGER->GetEventFromMemory("PauseEvent"));
			pPauseEvent->mPause = false;
			EVENT_MANAGER->BroadcastEvent(pPauseEvent);
		}
		/*else if(mSceneStack.front() == "DigiPenLevel")
		{
			PushScene("PauseMenu");
			ObjectFactory::GetInstance()->LoadLevel("PauseMenu");
		}
		else
		{
			ClearLevel(mSceneStack.front());
		}*/
	}

	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_W))
	{
		Cheats(mWorldID);
	}
	if (INPUT_MANAGER->IsPressed(SDL_SCANCODE_LCTRL) && INPUT_MANAGER->IsTriggered(SDL_SCANCODE_A))
	{
		Cheats(mWorldID,true);
	}

}

GEAR_API void SceneManager::LoadScene(std::string aScene)
{
	if (mScenes.find(aScene) == mScenes.end())
		mScenes.emplace(aScene, std::list<int>());
}

void SceneManager::LoadActionScene(std::string aActionScene)
{
	//ClearLevel(GetCurrent());
	mCurrentLevel = aActionScene;
	if (mCurrentLevel == "DigiPenLevel")
	{
		InitSceneLoad();
	}
	else
	{
		PushScene(mCurrentLevel);
		ObjectFactory::GetInstance()->LoadLevel(mCurrentLevel);
	}
}

void SceneManager::PushScene(std::string aLevel)
{
	if (mScenes.find(aLevel) == mScenes.end()) 
		LoadScene(aLevel);
	/*if (mSceneStack.size() > 0) {
		mSceneStack.pop_front();
	}*/
	mSceneStack.push_front(aLevel);

}

void SceneManager::SwapScene(std::string aLevel)
{
	if (mScenes.find(aLevel) == mScenes.end())
		LoadScene(aLevel);
	if (mSceneStack.size() > 0) {
		//reset level
		mSceneStack.pop_front();
	}
	mSceneStack.push_front(aLevel);
}

int SceneManager::PopScene()
{
	mSceneStack.pop_front();
	return -1;
}

GEAR_API std::list<int> SceneManager::PeekScene()
{
	std::string sceneName = GetCurrent();
	std::list<int> sceneIds;
	if (mScenes.find(sceneName) != mScenes.end())
		sceneIds = mScenes.at(sceneName);
	return sceneIds;
}

GEAR_API int SceneManager::AddEntity(int aId, std::string a_scene)
{
	std::string sceneName = a_scene;
	if (a_scene.empty()) 
		sceneName = GetCurrent();
	
	if (mScenes.find(sceneName) != mScenes.end()) {
		mScenes.at(sceneName).push_back(aId);
		return 1;
	} 
	else
		return 0;
}

GEAR_API int SceneManager::RemoveEntity(int aId)
{
	std::string sceneName = GetCurrent();

	std::list<int> sceneIds;
	if (mScenes.find(sceneName) != mScenes.end()) {
		mScenes.at(sceneName).erase(std::remove(mScenes.at(sceneName).begin(), mScenes.at(sceneName).end(), aId), mScenes.at(sceneName).end());
		return 1;
	}
	return 0;
}

GEAR_API std::string SceneManager::GetCurrent()
{
	std::string current = "";
	if (mSceneStack.size() > 0)
		current = mSceneStack.front();
	return current;
}

SceneManager::SceneManager() : mSceneID(-1)
{
}


SceneManager::~SceneManager()
{
}

SceneManager * SceneManager::GetInstance()
{
	if (!mInstance)
		mInstance = new SceneManager();
	return mInstance;
}

GEAR_API void SceneManager::Init()
{
	if (!mInstance)
		mInstance = new SceneManager();
	
	ReadLevelData();
	SetColor();
	mScreenHeight = Engine::GetInstance()->mWindowHeight;
	mScreenWidth = Engine::GetInstance()->mWindowWidth;
	mCompletedWorld = 0;

}

void SceneManager::Cleanup()
{
	WriteDataToSaveFile();
	delete mInstance;
}


void SceneManager::InitSceneLoad() {
	// Creates and loads entities and their components from specified level file
	SCENE_MANAGER->PushScene(mCurrentLevel);
	ObjectFactory::GetInstance()->LoadLevel(mCurrentLevel);
	ENTITY_MANAGER->ClearRuntimeLists();
	COMPONENT_MANAGER->ClearRuntimeLists();
}

std::vector<LevelData>* SceneManager::GetLevelData()
{
	return &mLevelData;
}

std::unordered_map<char, glm::vec3> SceneManager::GetColorData()
{
	return mGradeColor;
}

GEAR_API void SceneManager::ReadLevelData()
{
	Text* pLevelText = RESOURCE_MANAGER->LoadText("UnlockLevelData");

	std::string sdata;
	int i = 1;
	while (std::getline(pLevelText->mFileStream, sdata)) {

		std::istringstream stringStream(sdata);
		//std::pair<bool, std::pair<int,int>> data;

		std::string worldNum;
		std::getline(stringStream, worldNum, ',');

		std::string levelName;
		std::getline(stringStream, levelName, ',');

		std::string isUnlock;
		std::getline(stringStream, isUnlock, ',');
		const char *temp = isUnlock.c_str();

		std::string unlockScore;
		std::getline(stringStream, unlockScore, ',');

		std::string scoreData;
		std::getline(stringStream, scoreData, ',');

		std::string grade;
		std::getline(stringStream, grade, ',');
		char g = grade[0];

		bool unlock = temp[0] == 'Y' ? true : false;

		LevelData data(std::stoi(worldNum), levelName, i, unlock, std::stoi(unlockScore), std::stoi(scoreData), g);
		mLevelData.push_back(data);
		i++;
	}

	pLevelText->mFileStream.clear();
	pLevelText->mFileStream.seekg(0, std::ios::beg);
}

GEAR_API void SceneManager::WriteDataToSaveFile()
{
	std::ofstream savefile;
	savefile.open(".\\Resources\\Text\\UnlockLevelData.txt");

	std::vector<LevelData> *levelData = GetLevelData();

	for (LevelData &element : *levelData)
	{
		savefile << element.worldNumber << ",";
		savefile << element.LevelName << ",";
		char temp = element.isUnlocked == true ? 'Y' : 'N';
		savefile << temp << ",";
		savefile << element.scoreToUnlock << ",";
		savefile << element.highScore << ",";
		savefile << element.grade << "\n";
	}
	savefile.close();
}

void SceneManager::SetColor()
{
	mGradeColor.insert({ 'A', glm::vec3(255,223,0) });
	mGradeColor.insert({ 'B', glm::vec3(0,7,248) });
	mGradeColor.insert({ 'C', glm::vec3(100,59,0) });
	mGradeColor.insert({ 'D', glm::vec3(255,0,0) });
	//mGradeColor.insert({ 'D', glm::vec3(150,146,146) });


}

void SceneManager::SetWorldID(int aWorldID)
{
	mWorldID = aWorldID;
}

void SceneManager::Cheats(int worldID, bool aAll)
{
	std::vector<LevelData> *levelData = GetLevelData();

	for (LevelData &element : *levelData)
	{
		if (!aAll) {
			if (element.worldNumber == mWorldID)
			{
				element.isUnlocked = true;
				element.grade = element.grade == 'X' ? 'F' : element.grade;
			}
		}
		else {
			element.isUnlocked = true;
			element.grade = element.grade == 'X' ? 'F' : element.grade;
		}
	}
}

void SceneManager::ClearLevel(std::string a_SceneName)
{
	std::list<int> sceneIds;

	std::unordered_map<std::string, std::list<int>>::iterator sceneItr = mScenes.find(a_SceneName);
	if (sceneItr != mScenes.end()) {
		sceneIds = mScenes.at(a_SceneName);
		std::list<int>::iterator begin = sceneIds.begin();
		std::list<int>::iterator end = sceneIds.end();
		for (std::list<int>::iterator itr = begin; itr != end; ++itr)
		{
			ENTITY_MANAGER->DeleteEntity(*itr);
		}
		mScenes.erase(sceneItr);
		mSceneStack.remove(a_SceneName);
	}
}

void SceneManager::ClearAll()
{
	std::list<std::string> tempScenes = mSceneStack;
	for (std::list<std::string>::iterator itr = tempScenes.begin(); itr != tempScenes.end(); ++itr)
	{
		ClearLevel(*itr);
	}
}
