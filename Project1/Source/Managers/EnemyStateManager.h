#pragma once
class Entity;
class Transform;

#define ENEMY_STATE_MANAGER EnemyStateManager::GetInstance()

class EnemyStateManager
{
private:
	GEAR_API EnemyStateManager();
	GEAR_API ~EnemyStateManager();
	static EnemyStateManager* mInstance;

public:
	/*!
	 * Get the EnemyStateManager Singleton reference
	 *
	 * \return The EnemyStateManager reference
	 */
	GEAR_API static EnemyStateManager* GetInstance();
	/*!
	 * Initialize the EnemyStateManger
	 * Sets the spwan point locations for enemies to navigate
	 *
	 */
	GEAR_API void Init();
	/*!
	 * 
	 *
	 */
	GEAR_API void CleanUp();

	/*!
	 *
	 *
	 */
	GEAR_API void Update(float);

	/*! Spawn Enemy after certain interval of time
	 *
	 *
	 */
	GEAR_API void SpawnEnemy(int);

	/*! Get the spwan point location of the memory
	 *
	 * \return the location(glm::vec3)
	 */
	GEAR_API inline std::vector<glm::vec3> GetSpawnPoints() { return mSpawnPoints; }

	int mEnemiesKilled;
	bool mGameOver;
private:
	int mNumberOfEnemies;
	int mNumberOfSpawnPoints;
	int mMaxEnemies;
	int mTotalEnemiesSpawned;

	int mAlive;

	float mdeltaTime;

	std::vector<glm::vec3> mSpawnPoints;

	Entity* mCar;
	Transform* mCarTrans;


};

