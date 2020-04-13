#pragma once
#include "Core.h"
#define SCENE_MANAGER SceneManager::GetInstance()

struct LevelData {
	std::string LevelName;
	int LevelNumber;
	bool isUnlocked;
	int scoreToUnlock;
	int highScore;
	char grade;
	int worldNumber;

	LevelData(int _worldNumber, std::string Lname, int num, bool isUnlock, int scoreUnlock, int Hscore, char _grade) :
		worldNumber(_worldNumber),LevelName(Lname), LevelNumber(num), isUnlocked(isUnlock), scoreToUnlock(scoreUnlock), highScore(Hscore),
		grade(_grade)
	{}
};

class Component;

class SceneManager
{
public:
	GEAR_API static SceneManager* GetInstance();
	/** 
	*/
	GEAR_API void Init();

	/** 
	*/
	GEAR_API void Cleanup();
	GEAR_API void LoadScene(std::string aScene);
	GEAR_API void LoadActionScene(std::string aActionScene);
	GEAR_API void PushScene(std::string aScene);
	GEAR_API void SwapScene(std::string aScene);
	GEAR_API void Update();
	GEAR_API int PopScene();
	GEAR_API std::list<int> PeekScene();
	GEAR_API int AddEntity(int aId, std::string a_scene = "");
	GEAR_API int RemoveEntity(int aId);
	GEAR_API void ClearLevel(std::string a_scene);
	GEAR_API void ClearAll();
	GEAR_API std::string GetCurrent();

	GEAR_API void InitSceneLoad();
	inline void SetCurrentLevel(std::string aLevel) { mCurrentLevel = aLevel; }

	GEAR_API std::vector<LevelData>* GetLevelData();
	GEAR_API std::unordered_map<char, glm::vec3> GetColorData();

	GEAR_API void ReadLevelData();
	GEAR_API void WriteDataToSaveFile();
	GEAR_API void SetColor();

	GEAR_API void SetWorldID(int aWorldID);
	GEAR_API void Cheats(int worldID = 1, bool aAll = false);
	GEAR_API inline int GetCompletedWorld() { return mCompletedWorld; }
	GEAR_API inline void SetCompletedWorld(int a_value) { mCompletedWorld = a_value; }

	int mSceneID;
	int mScreenWidth;
	int mScreenHeight;
private:
	
	GEAR_API SceneManager();
	GEAR_API ~SceneManager();

private:
	static SceneManager* mInstance;
	std::unordered_map<std::string,std::list<int>> mScenes;
	std::list<std::string> mSceneStack;
	std::string mCurrentLevel;
	std::vector<LevelData> mLevelData;
	std::unordered_map<char, glm::vec3> mGradeColor;
	int mWorldID;
	int mCompletedWorld;
};

