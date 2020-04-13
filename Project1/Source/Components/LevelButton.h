#pragma once

#include "Component.h"

class SpotLight;
class Entity;
class LocalLightComponent;

class LevelButton : public Component
{
public:
	GEAR_API LevelButton();
	GEAR_API virtual ~LevelButton();

	GEAR_API virtual void Init();
	GEAR_API void Reset();
	GEAR_API LevelButton* Create();

	GEAR_API void Update(float a_deltaTime);
	GEAR_API void DisplayDebug();

	GEAR_API void Serialize(Json::Value a_root);
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);

	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Clone(int objID);
		void SetupLevel(std::string a_levelName);

	int mLevelID;
	int mHighScore;
	bool mIsUnlocked;
	int mUnlockScore;
	std::string mLevelName;
	bool in;
	int mWorldID;

private:
	int mChildID;
	bool mOnce;
	LocalLightComponent *mChildLight;
	Entity *mScale;
	char mGrade;
	int tempID;
	Transform* mTransScale;
	int mScaleID;
};