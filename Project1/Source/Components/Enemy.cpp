#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Enemy.h"

#include "../Engine.h"
#include "../Managers/ComponentManager.h"
#include "../Managers/EntityManager.h"
#include "../Managers/EnemyStateManager.h"
#include "Physics/PhysicsManager.h"
#include "../Editor/RayCast.h"
#include "Transform.h"
#include "Collider.h"
#include "Audio.h"
#include "../Entity.h"
#include "Bullet.h"
#include "../Random/RandomNumberGenerator.h"

Enemy::Enemy() : Component("Enemy")
{
	velocity = 0.1f;
	timer = 0.0f;
	fire = false;
	mRay = new Ray();
	currentPoint = 0;
	hideTime = 0.0f;
	once = false;
	mHealth = 100;
}

 Enemy::~Enemy()
{

}

 void Enemy::Reset()
{

}

 Enemy* Enemy::Create()
{
	return new Enemy();
}

 void Enemy::Update(float a_deltaTime)
{
	 timer += a_deltaTime / 1000.f;

	 if (currentState == BASE_POS)
	 {
		 Transform* enemyTrans = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");

		 glm::vec3 tran = enemyTrans->GetPosition();

		 startPos = tran;

		 endPos = spawnPoints[currentPoint];

		 direction = glm::normalize(endPos - startPos);

		 tran += direction * velocity;

		 glm::vec3 xyz = endPos - tran;

		 float sqDist = xyz.x + xyz.y + xyz.z;

		 if (abs(sqDist) < 0.25)
		 {
			 currentState = FIND;
		 }

		 enemyTrans->SetPosition(tran);
	 }



	 if (currentState == FIND) {
	//	 LOG_INFO("FIND");
		 //MOVE 
		 int randNum = RandomNumberGenerator::GetInstance()->GenerateInteger(0, 100);

		 switch (currentPoint)
		 {
		 case 0:
			 if (randNum < 30 )							nextPoint = 1;
			 else if (randNum >= 30 && randNum < 60)	nextPoint = 2;
			 else if (randNum >= 60)					nextPoint = 4;
			 currentState = CALCULATE;
			 break;
		 case 1:
			 if (randNum < 30)							nextPoint = 0;
			 else if (randNum >= 30 && randNum < 60)	nextPoint = 2;
			 else if (randNum >= 60)					nextPoint = 3;
			 currentState = CALCULATE;
			 break;
		 case 2:
			 if (randNum < 25)							nextPoint = 0;
			 else if (randNum >= 25 && randNum < 50)	nextPoint = 1;
			 else if (randNum >= 50 && randNum < 70)	nextPoint = 3;
			 else if (randNum >= 75)					nextPoint = 4;
			 currentState = CALCULATE;
			 break;
		 case 3:
			 if (randNum < 30)							nextPoint = 2;
			 else if (randNum >= 30 && randNum < 60)	nextPoint = 4;
			 else if (randNum >= 60)					nextPoint = 5;
			 currentState = CALCULATE;
			 break;
		 case 4:
			 if (randNum < 30)							nextPoint = 2;
			 else if (randNum >= 30 && randNum < 60)	nextPoint = 3;
			 else if (randNum >= 60)					nextPoint = 4;
			 currentState = CALCULATE;
			 break;
		 case 5:
			 if (randNum < 25)							nextPoint = 3;
			 else if (randNum >= 25 && randNum < 50)	nextPoint = 4;
			 else if (randNum >= 50 && randNum < 70)	nextPoint = 5;
			 else if (randNum >= 75)					nextPoint = 7;
			 currentState = CALCULATE;
			 break;
		 case 6:
			 if (randNum < 50)			nextPoint = 5;
			 else if (randNum >= 50)	nextPoint = 7;
			 currentState = CALCULATE;
			 break;
		 case 7:
			 if (randNum < 50)			nextPoint = 5;
			 else if (randNum >= 50)	nextPoint = 6;
			 currentState = CALCULATE;
			 break;
		 default:
			 break;
		 }
	 }

	 if (currentState == CALCULATE)
	 {
		// LOG_INFO("CALCULATE");

		 Transform* enemyTrans = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
		 startPos = enemyTrans->GetPosition();
		 endPos = spawnPoints[nextPoint];
		 direction = glm::normalize(endPos - startPos);

		 currentState = MOVE;
	 }

	 if (currentState == MOVE)
	 {
		// LOG_INFO("MOVE");

		 Transform* enemyTrans = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
		 glm::vec3 tempTrans = enemyTrans->GetPosition();
		 tempTrans += direction /** (timer)* */* velocity;

		 glm::vec3 xyz = endPos - tempTrans;

		 float sqDist = xyz.x + xyz.y + xyz.z;

		 if (abs(sqDist) < 0.25)
		 {
			 currentPoint = nextPoint;
			 currentState = HIDE;
		 }

		 enemyTrans->SetPosition(tempTrans);

		// std::cout << "Current PT" << currentPoint << std::endl;
		// std::cout << "NEXt pt" << nextPoint << std::endl;

	 }

	 if (currentState == HIDE)
	 {
		// LOG_INFO("HIDE");
		 glm::vec3 tempEnemypos = mPos->GetPosition();
		 //HIDE or SHOOT
		 hideTime += a_deltaTime / 1000.0f;
		 if (hideTime <= 2.0f)
		 {
			 if(!once)
				 tempEnemypos.y -= 1.0f;
			 once = true;
			 mPos->SetPosition(tempEnemypos);
		 }
		 else {
			 tempEnemypos.y += 1.0f;
			 once = false;
			 hideTime = 0.0f;
			 mPos->SetPosition(tempEnemypos);

			 //find or shoot 
			 int randNum = RandomNumberGenerator::GetInstance()->GenerateInteger(0, 100);
			 if (randNum >= 50)
				 currentState = SHOOT;
			 else
				currentState = FIND;
		 }
	 }


	 //FIRE
	 //if (timer >= 5.0f)
	 if(currentState == SHOOT)
	 {
		 // LOG_INFO("SHOOT");
		 Entity* mBullet = ENTITY_MANAGER->CreateRuntimeEntity("Bullet");
		 std::vector<Component*> components = COMPONENT_MANAGER->GetRuntimeComponents(mBullet->GetID());

		 for (Component* compo : components)
		 {
			 compo->Init();
		 }

		 Transform* bulletTrans = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mBullet->GetID(), "Transform");
		 Bullet* mBulletComp = COMPONENT_MANAGER->GetRuntimeComponent<Bullet>(mBullet->GetID(), "BulletComp");
		 mBulletComp->owner = "Enemy";

		 std::vector<Entity*> list = ENTITY_MANAGER->GetEntities("PlayerObject");
		 for (Entity* element : list)
		 {
			 playerPos = COMPONENT_MANAGER->GetComponent<Transform>(element->GetID(), "Transform");
		 }

		 mBulletComp->mStartPos = mPos->GetPosition();
		 mBulletComp->mStartPos.y += 1.5f;
	//	 mBulletComp->mStartPos.z += 1.5f;

		// mRay->origin = mBulletComp->mStartPos;

		 glm::vec3 tempBulletTrans = bulletTrans->GetPosition();
		 tempBulletTrans = mBulletComp->mStartPos;
		 bulletTrans->SetPosition(tempBulletTrans);

		 glm::vec3 tempPlayerPos = playerPos->GetPosition();
		 tempPlayerPos.y += 1.0f;

		 mBulletComp->mDirection = glm::normalize(tempPlayerPos - mBulletComp->mStartPos);

		// timer = 0.0f;
		 currentState = FIND;
	 }

	 if (ENEMY_STATE_MANAGER->mGameOver == true)
	 {
		 currentState = STOP;
	 }

	 if (currentState == STOP)
	 {
		 //LOG_INFO("DONE");
	 }

}

 void Enemy::DisplayDebug()
{

}

 void Enemy::Serialize(Json::Value a_root)
{

}

 void Enemy::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{

}

 void Enemy::Destroy()
{
	 ENEMY_STATE_MANAGER->mEnemiesKilled++;
	 ENTITY_MANAGER->DeleteEntity(mOwnerID);
}

 void Enemy::Init()
{
	 mPos = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
	 if (!mPos)
		 mPos = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");

	 mCollid = COMPONENT_MANAGER->GetComponent<Collider>(mOwnerID, "Collider");
	 if (!mCollid)
		 mCollid = COMPONENT_MANAGER->GetRuntimeComponent<Collider>(mOwnerID, "Collider");

	 mPos = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
	 if (!mPos)
		 mPos = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");

	 mAudio = COMPONENT_MANAGER->GetComponent<Audio>(mOwnerID, "Audio");
	 if (!mAudio)
		 mAudio = COMPONENT_MANAGER->GetRuntimeComponent<Audio>(mOwnerID, "Audio");

	 spawnPoints = ENEMY_STATE_MANAGER->GetSpawnPoints();

	// pathMap.emplace_back(std::make_pair(0,));
	 currentState = BASE_POS;
}

 void Enemy::HandleEvent(Event* a_pEvent)
{

}
