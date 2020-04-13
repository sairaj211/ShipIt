#pragma once
#include "Component.h"
class Transform;
class Bullet;
class Collider;
class Ray;
class Audio;


enum STATES {
	BASE_POS = 0,
	FIND,
	CALCULATE,
	MOVE,
	HIDE,
	SHOOT,
	STOP
};




class Enemy: public Component
{
public:
	GEAR_API void Clone(int objID) {}
	GEAR_API Enemy();
	GEAR_API virtual ~Enemy();
	GEAR_API void Reset();

	GEAR_API Enemy* Create();

	GEAR_API void Update(float a_deltaTime);

	GEAR_API void DisplayDebug();

	GEAR_API void Serialize(Json::Value a_root);

	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);

	GEAR_API void Destroy();

	GEAR_API virtual void Init();

	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	int currentPoint;
	int nextPoint;
	int mHealth;

private:
	Transform* mPos;		//start pos
	Transform* playerPos;	//end pos
	Collider *mCollid;
	Audio* mAudio;

	glm::vec3 direction;
	float velocity;

	float timer;
	bool fire;
	Ray* mRay;

	STATES currentState;
	glm::vec3 startPos;
	glm::vec3 endPos;


	std::vector<glm::vec3> spawnPoints;

	float hideTime;
	bool once;
	Collider* isBulletCollider;

	//std::vector<std::pair<int, std::vector<int>>> pathMap;

};

