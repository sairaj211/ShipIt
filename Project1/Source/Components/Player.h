#pragma once

#include "Components/Body.h"
#include "Components/Collider.h"
#include "Components/Transform.h"
#include "Physics/PhysicsManager.h"

class Player : public Component {

public:
	GEAR_API Player();
	GEAR_API ~Player();
	GEAR_API void Init();
	GEAR_API virtual Player* Create();
	GEAR_API void Update(float);
	GEAR_API void ConstrainVelocityPlayer();
	GEAR_API void CheckGroundedPlayer();
	GEAR_API void DisplayDebug();
	GEAR_API void Serialize(Json::Value);
	GEAR_API void DeSerialize(Json::Value &a_root, bool a_ignoreDefault = false);
	/*!
	 * Move player by x, y velocity
	 * 
	 * \param aX amount to change the velocity in the x direction
	 * \param aY amount to change the velocity in the y direction
	 */
	GEAR_API void Move(float aX, float aY, bool sprint =false);
	/*!
	 * If the player is on the ground cause the player
	 * to jump by an amount
	 * 
	 * \param aJumpForce
	 */
	GEAR_API void Jump(float aJumpForce);
	/*!
	 * Rotate the player by an ammount
	 * 
	 * \param aAngle
	 */
	GEAR_API void Rotate(float aAngle);
	/*!
	 * Set the max value for the player velocity
	 * 
	 * \param aMax  Max Velocity
	 */
	GEAR_API void SetMaxVelocity(float aMax);
	/*!
	 * Get if the player is on the ground
	 * 
	 * \return true if the player is on the ground or not
	 */
	GEAR_API bool GetIsGrounded();

	GEAR_API void Reset();

	GEAR_API void MakeAttack();

	/*!
	 *
	 *
	 * \param a_pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	GEAR_API void Clone(int objID);
	

private:
	Transform* playerTransform;
	bool mInitialized;
public:
	float mAngle;
	glm::vec3 bOffset = glm::vec3(0, 0, 0);/*!<    */
	glm::vec3 velPlayer = glm::vec3(0, 0, 0);/*!<    */
	bool mAttack = false;
	float mRecharge;
	float mHitRecharge;
	float mRechargeTime;
	float mHitTime;
	bool gnd = true;/*!< Player is Grounded */
	float mMaxVelocity;/*!< Player max velocity in any direction   */
	std::unordered_map<std::string, float> mPowerUps;
	float mFiringRate;

	float currentDelta;
	float PrevDelta;
	int mKilled;
};