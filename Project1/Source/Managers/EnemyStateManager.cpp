#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "../Engine.h"
#include "EnemyStateManager.h"
#include "../Entity.h"
#include "Components/Transform.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "../Components/Enemy.h"
#include "../Components/Transform.h"

#include "../Random/RandomNumberGenerator.h"

EnemyStateManager* EnemyStateManager::mInstance = nullptr;

EnemyStateManager::EnemyStateManager()
{
}


EnemyStateManager::~EnemyStateManager()
{
}

EnemyStateManager* EnemyStateManager::GetInstance()
{
	if (!mInstance)
	{
		mInstance = new EnemyStateManager();
	}
	return mInstance;
}

void EnemyStateManager::Init()
{
	//mNumberOfEnemies = 1;
	mMaxEnemies = 6; 
	mNumberOfSpawnPoints = 1;
	mEnemiesKilled = 0;
	mTotalEnemiesSpawned = 0;
	mdeltaTime = 0.0f;

	mSpawnPoints.push_back(glm::vec3(-9.0f, 0.0f, -8.0f));
	mSpawnPoints.push_back(glm::vec3( 9.0f, 0.0f, -8.0f));
	mSpawnPoints.push_back(glm::vec3( 1.0f, 0.0f, -12.0f));
	mSpawnPoints.push_back(glm::vec3(-9.0f, 0.0f, -22.0f));
	
	mSpawnPoints.push_back(glm::vec3( 9.0f, 0.0f, -22.0f));
	mSpawnPoints.push_back(glm::vec3( 1.0f, 0.0f, -28.0f));
	mSpawnPoints.push_back(glm::vec3(-9.0f, 0.0f, -34.0f));
	mSpawnPoints.push_back(glm::vec3( 9.0f, 0.0f, -34.0f));

	mCar = ENTITY_MANAGER->CreateRuntimeEntity("Car");
	std::vector<Component*> components = COMPONENT_MANAGER->GetRuntimeComponents(mCar->GetID());

	for (Component* compo : components)
	{
		compo->Init();
	}
	mCarTrans = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mCar->GetID(), "Transform");

	mGameOver = false;
}

void EnemyStateManager::CleanUp()
{
	delete mInstance;
}

void EnemyStateManager::Update(float deltaTime)
{
	//TEST

	glm::vec3 tempPos = mCarTrans->GetPosition();

   	if(tempPos.x < 0.0f && mTotalEnemiesSpawned < mMaxEnemies)
   	{
   		mdeltaTime += deltaTime / 1000.0f;
   		//Update all alive enemies or spawn enemies if not present
   		// if total enemy count is less then max spawn enemies
  		if (mdeltaTime >= 1.0f && mTotalEnemiesSpawned < mMaxEnemies)
  		{
  			SpawnEnemy(mTotalEnemiesSpawned);
  			++mTotalEnemiesSpawned;
  
  			mdeltaTime = 0.0f;
  		}
   	}
   	else
   	{
   		tempPos.x -= 0.7f;
		mCarTrans->SetPosition(tempPos);

		glm::vec3 tempRot = mCarTrans->GetRotationAngles();
		tempRot.y += 2.0f;
		mCarTrans->SetRotationAngles(tempRot);
   	}



// 	//RESET
// 	if (mMaxEnemies % (mEnemiesKilled+1) == 1 )
// 	{
// 		mTotalEnemiesSpawned = 0;
// 		mdeltaTime = 0.0f;
// 	}

	//LOG_INFO(mTotalEnemiesSpawned);

}

void EnemyStateManager::SpawnEnemy(int number)
{
	Entity* temp = ENTITY_MANAGER->CreateRuntimeEntity("EnemyObject");
	std::vector<Component*> components = COMPONENT_MANAGER->GetRuntimeComponents(temp->GetID());

	for (Component* compo : components)
	{
		compo->Init();
	}

	Transform* enemyTrans = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(temp->GetID(), "Transform");
	Enemy* enemyComp = COMPONENT_MANAGER->GetRuntimeComponent<Enemy>(temp->GetID(), "Enemy");
	//enemyTrans->mPosition = glm::vec3(-25.0f, 2.0f, -20.0f);

	//int random = RandomNumberGenerator::GetInstance()->GenerateInteger(0,100);
	int randomPos = RandomNumberGenerator::GetInstance()->GenerateInteger(5, 7);

	glm::vec3 tempPos = enemyTrans->GetPosition();
	tempPos = mSpawnPoints[randomPos];
	enemyComp->currentPoint = randomPos;


	//DROP point 
	tempPos = glm::vec3(0.0f, 0.0f, -60.f);

	enemyTrans->SetPosition(tempPos);
}
