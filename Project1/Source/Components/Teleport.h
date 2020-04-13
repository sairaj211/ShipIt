#pragma once

#include "Component.h"

class Teleport : public Component
{
public:
	GEAR_API Teleport();
	GEAR_API virtual ~Teleport();

	GEAR_API virtual void Init();
	GEAR_API void Reset();
	GEAR_API Teleport* Create();

	GEAR_API void Update(float a_deltaTime);
	GEAR_API void DisplayDebug();

	GEAR_API void Serialize(Json::Value a_root);
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);

	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Clone(int objID);

	int mPairID;
private:
	bool mUpdatePairs;
	int mOtherObjectID;
	std::string mDirection;
};