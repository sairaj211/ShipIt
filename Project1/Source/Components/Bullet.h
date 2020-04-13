#pragma once

#include "Component.h"

class Transform;
class Collider;
class Ray;
class Audio;
class ModelComponent;
class ParticlesComponent;

class Bullet : public Component
{
public:
	/*!
	 * Constructor for bullet class used by player and enemy object
	 *
	 * \return
	 */
	GEAR_API void Clone(int objID) {}
	GEAR_API Bullet ();

	/*!
	 * 
	 *
	 * \return
	 */
	GEAR_API virtual ~Bullet();

	/*!
	 * Reset all member variables
	 *
	 * \return
	 */
	GEAR_API void Reset();

	/*!
	 * Creates new bullet component from memory
	 *
	 * \return
	 */

	GEAR_API Bullet* Create();

	/*!
	* Updates bullet component every frame
	*
	* \return
	*/
	GEAR_API void Update(float a_deltaTime);

	/*!
	* Used to display debug information
	*
	* \return
	*/
	GEAR_API void DisplayDebug();

	/*!
	 * Used by scripts. which converts glm::vec to 3 floats
	 * Initialize the Bullet with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the Bullet data
	 */
	GEAR_API void Serialize(Json::Value a_root);

	/*!
	 * Save the Bullet data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the Bullet data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);

	/*!
	 * Deletes the component from object's component list
	 *
	 * \return
	 */
	GEAR_API void Destroy();

	/*!
	 * To initialize all member variables
	 *
	 * \return
	 */
	GEAR_API virtual void Init();

	/*!
	 * Handle the given event according to its
	 * type
	 *
	 * \param a_pEvent The event being handled by the Component
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	/*!
	 * Get the owner of current bullet (eg . player or enemy)
	 *
	 * \return
	 */
	GEAR_API inline std::string GetOwner() { return owner; }

public:
	glm::vec3 mStartPos;
	glm::vec3 mEndPos;
	glm::vec3 mDirection;

	float mVelocity;
	float mStartTimer;
	float mMaxTime;
	std::string owner;
	std::string type;

	glm::vec3 rockVelocity;
private:
	Transform* mBulletTrans;
	Ray* mRay;
	Collider* mCollid;
	Audio* mAudio;
	ModelComponent* mModel;
	ParticlesComponent* mParticle;

	ParticlesComponent* mGrenadeParticle;


	float mGTimer;


	bool mPlayOnce;
};

