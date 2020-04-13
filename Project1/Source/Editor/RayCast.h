#pragma once

class Collider;
class Transform;
class Entity;
class Bullet;
class Body;

class Ray
{
public:
	Ray() {}
	~Ray() {}
private:
public:
	glm::vec3 origin;
	glm::vec3 direction;
private:
};

class RayCast
{
public:
	GEAR_API static RayCast* GetInstance();
	GEAR_API void Init();
	GEAR_API void CleanUp();
	GEAR_API int CastRay(float x, float y, bool isBullet);
	GEAR_API void ShootBullet(std::pair<Collider*, glm::vec3>);
	GEAR_API void Grenade(std::pair<Collider*, glm::vec3>);
	GEAR_API void MoveCrosshair(float x, float y);
	GEAR_API void MoveBullet();
	GEAR_API void IdentifyBullet();
private:
	GEAR_API RayCast();
	GEAR_API ~RayCast();
public:
	Ray *  mRay;/*<! */
	Collider * mCrossHair = nullptr;
	Collider * mBulletCollider = nullptr;
	Transform * mBuletTransform = nullptr;
	glm::vec3 mBulletOrigin = glm::vec3(0, 0, 0);
	glm::vec3 mBulletDir = glm::vec3(0, 0, 0);
private:
	static RayCast* mInstance;

public:
	int id;
	int mBulletState = 0;
	glm::vec3 bulletO;//Origin
	glm::vec3 bulletD;//Destiny


	Entity* playerEntity;
	Transform* objTrans;
	Bullet* objBullet;
	Body* objBody;

	Transform* playerTrans;




};