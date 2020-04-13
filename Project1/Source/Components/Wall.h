#pragma once

#include "Component.h"

class Transform;
class Collider;
class Ray;
class Audio;
class ModelComponent;
class ParticlesComponent;

class Wall : public Component
{
public:
		/*!
	 * Constructor for Wall class 
	 *
	 * \return
	 */
	GEAR_API Wall();

	GEAR_API virtual ~Wall();


	/*!
	 * Reset all member variables
	 *
	 * \return
	 */
	GEAR_API void Reset();

	/*!
	 * Creates new Wall component from memory
	 *
	 * \return
	 */
	GEAR_API Wall* Create();


	/*!
	* Updates Wall component every frame
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
	 * Initialize the Wall with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the Wall data
	 */
	GEAR_API void Serialize(Json::Value a_root);

	/*!
	 * Save the Bullet data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the Wall data is being saved to
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
	 * Change the color of the wall after taking certain amount of damage
	 *
	 * \return
	 */
	GEAR_API void ChangeColor(std::string);

	GEAR_API void Clone(int objID) {}

	int mHealth;
	std::string mType;
	int mTotalKills;
private:
	ModelComponent* mwallModel;
	Collider* mWallCollider;
	Collider* mIsBulletCollider;
	Transform* mWallTransform;
	glm::vec3 mPos;
};

