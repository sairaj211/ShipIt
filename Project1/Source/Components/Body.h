#pragma once

#include "Transform.h"
#include "Managers/ComponentManager.h" //to traverse on ComponentMap

/*! 
 * \class Body
 *  \brief manages forces , velocities and accelerations of the gameObject
 *  CollisionManager takes variables of this  class .to calculate Impulses, and other forces related to the collision
 *
 */
class Body : public Component {

public:
	/*! Constructor
	 */
	GEAR_API Body();
	/*! Destructor
	 */
	GEAR_API ~Body();

	/*! modify velocities based on delta tine
	 * \param float _fixedDeltaTime
	 * \param vec3 is the velocity to force.
	 */
	GEAR_API void UpdateGravityForces(float a_fixedDeltaTime, glm::vec3 a_gravity);

	/*! modify postions based on delta time
	 * \param float _fixedDeltaTime
	 */
	GEAR_API void UpdatePositions(float a_fixedDeltaTime);
	/*! modify velocities based on delta tine
	* \param linear is the velocity to be added.
	* \param angular is the angular velocity to add
	 */
	GEAR_API void AddVelocities(glm::vec3 vel, glm::vec3 p, bool useMass);
	/*! appy force to the body
	 * \param vec3 F the Force
	 * \param vec3 p point force is applied.
	 */
	GEAR_API void ApplyForce(glm::vec3 F, glm::vec3 p);
	/*!
	 *
	 * saves last position, and based on Euler. updates position ,related to Vel
	 * \param based on DeltaTime
	 */
	GEAR_API void Integrate(float);
	/*!  Return a new instance of the Body
	 * \return  refeerence to a new Body
	 */
	GEAR_API Body *Create();
	/*!  Actions Related to the Bodies.
	 * \param  deltaTime
	 */
	GEAR_API void Update(float);
	/*! Expose variables for The Editor
	 */
	GEAR_API void DisplayDebug();
	/*! solve Inv Mas,  get references of self transform, solve actPosition and prevPosition
	 */
	GEAR_API virtual void Init();
	/*! Reset positions, velocities and accelerations to 0  useGravity to false
	 */
	GEAR_API void SelfReset();
	/*! Call this to read all of the values of the body
	 * to the json indicated with the input
	 * \param the root for reading the 
	 */
	GEAR_API void Serialize(Json::Value);
	/*!  Call this to save all of the values of the body
	 * to the json indicated with the input
	 * \param the root for writing the component
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);

	GEAR_API void Reset();
	/*!
	 *
	 *
	 * \param a_pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);

	/*! DEPRECATED
		 * \param 
		 */
	GEAR_API  void Move(float);

	/*! if any axis is restricted, reset velocity on that axis to 0
	 */
	GEAR_API void ConstrainVelocity();

	

	/** if the variable is set .. force objectvelocity to the param
	 *  More details about this function.
	 * \param vec3 is the velocity to force.
	 */
	GEAR_API void SetConstantVelocity(glm::vec3);

	
	/*!
	 * Get the x,y,z of a body
	 *
	 * \return the x,y,z velocity
	 */
	GEAR_API std::tuple<float, float, float> GetVelocity();

	GEAR_API void SetVelocity(float a_x, float a_y, float a_z);

	GEAR_API void Clone(int id);

	GEAR_API glm::mat3 GetGlobalInverseTenser();

private:
	Transform* _transform;
public:
	glm::vec3 mPos; /*!<actual Position*/
	glm::vec3 mTheta; /*!<angular actual Position*/

	glm::vec3 mPrevPos;/*!<previous position*/
	glm::vec3 mPrevTheta;/*!< angular previous position*/

	glm::vec3 mVel;/*!<velocity*/
	glm::vec3 mOmega;/*!<angular velocity*/

	glm::vec3 mAcc;/*!<acceleration*/
	glm::vec3 mAlpha;/*!<angular acceleration*/

	glm::vec3 mLinearForce;/*!<linear force applied to the body*/
	glm::vec3 mTorque;/*!< torque applied to the body*/

	//glm::vec3 mConstantVelocity;/*!<if is constant velocity set, then this is the velocity*/

	float mMass;  /*! if 0 ,  Body will not be affected by external forces */
	float mInvMass;/*!<inverse mass for linear calculation*/

	glm::mat3 mI_Body;
	glm::mat3 mInvI_Body;

	bool mApplyFriction;/*!<doxygen description of the member*/

	//bool mIsConstantVelocitySet;/*!<so far is not used*/

	bool mConstrainX;/*!<will it move on X*/
	bool mConstrainY;/*!<will it move on Y*/
	bool mConstrainZ;/*!<will it move on Z*/
	bool mUseGravity;/*!< will it use Gravity Forces*/
	bool mAllowRotations;/*!< will it use rotate based on forces*/



};