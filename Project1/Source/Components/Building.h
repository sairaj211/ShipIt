#pragma once

#include "Component.h"

class SpotLight;
class Entity;

class Building : public Component
{
public:
	GEAR_API Building();
	GEAR_API virtual ~Building();

	GEAR_API virtual void Init();
	GEAR_API void Reset();
	GEAR_API Building* Create();

	GEAR_API void Update(float a_deltaTime);
	GEAR_API void DisplayDebug();

	GEAR_API void Serialize(Json::Value a_root);
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);

	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Clone(int objID);

private:
	SpotLight *mChild;
	int mWorldID;
	bool once;
	int mChildID;
};