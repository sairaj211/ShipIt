#pragma once
#include "CollisionManager.h"
#include "Components/Collider.h"
#include "Managers/ComponentManager.h" //to traverse on ComponentMap
#include "Components/Body.h"
#include "Component.h"
#include "Shape.h"
#include "AABBTree.h"
#include <thread>
#include <shared_mutex>



/*! \class PhysicsManager
 *  \brief Manages all contacts, generate list of  Collisions/Triggers enter/exit/stay, gneerates Forces,  updates positions, corrects Positions
 * On Update constangly ask which objects are colliding, generates a list , and then. acts accordingly,
 *  helped by CollisionManager, to solve if some objects collide and what forces will be related,
 * also how to react to specific set of collition Elements
* If any collider is trigger, there is not anyImpulse,
 */
class PhysicsManager
{
public:
	/// singleton call
	GEAR_API static PhysicsManager * GetInstance();

	/** all Body2D objects, will have  mTotalForce = 0,0.
	*/
	GEAR_API void ResetForces();

	/** generates an instance of the CollisionManager
	*/
	GEAR_API void Init();

	/** delete ContactPoints, Contact2DManager, and self singleton
	*/
	GEAR_API void Cleanup();

	/** constantly do all functionality
	*/
	GEAR_API void Update(float);
	/*!
	 * Get the current collision manager
	 * 
	 * \return the current collision manager
	 */
	GEAR_API CollisionManager* GetCollisionManager();

	/** if  this collider participates into a  OnCollisionEnter, return true
	* \param _collider
	* \return if this collider participates into a  OnCollisionEnter, return true
	*/
	GEAR_API bool OnCollisionEnter(Collider *);
	/** if  this collider participates into a  OnCollisionExit, return true
	* \param _collider
	* \return if this collider participates into a  OnCollisionExit, return true
	*/
	GEAR_API bool OnCollisionExit(Collider *);
	/** if  this collider participates into a  OnCollisionStay, return true
	* \param _collider
	* \return if this collider participates into a  OnCollisionStay, return true
	*/
	GEAR_API bool OnCollisionStay(Collider *);
	/** if  this collider participates into a  OnTriggerEnter, return true
	* \param _collider
	* \return if this collider participates into a  OnTriggerEnter, return true
	*/
	GEAR_API bool OnTriggerEnter(Collider *);
	/** if  this collider participates into a  OnTriggerExit, return true
	* \param _collider
	* \return if this collider participates into a  OnTriggerExit, return true
	*/
	GEAR_API bool OnTriggerExit(Collider *);
	/** if  this collider participates into a  OnTriggerStay, return true
	* \param _collider
	* \return if this collider participates into a  OnTriggerStay, return true
	*/
	GEAR_API bool OnTriggerStay(Collider *);
	/** if  both colliders participate into a  OnCollisionEnter, return true
* \param _collider
* \return if this collider participates into a  OnCollisionEnter, return true
*/
	GEAR_API bool OnCollisionEnter(Collider *, Collider *);
	/** if both colliders participate into a  OnCollisionExit, return true
	* \param _collider
	* \return if both colliders participate into a  OnCollisionExit, return true
	*/
	GEAR_API bool OnCollisionExit(Collider *, Collider *);
	/** if both colliders participate into a  OnCollisionStay, return true
	* \param _collider
	* \return if both colliders participate into a  OnCollisionStay, return true
	*/
	GEAR_API bool OnCollisionStay(Collider * , Collider *);
	/** if both colliders participate into a  OnTriggerEnter, return true
	* \param _collider
	* \return if both colliders participate into a  OnTriggerEnter, return true
	*/
	GEAR_API bool OnTriggerEnter(Collider *, Collider *);
	/** if both colliders participate into a  OnTriggerExit, return true
	* \param _collider
	* \return if both colliders participate into a  OnTriggerExit, return true
	*/
	GEAR_API bool OnTriggerExit(Collider *, Collider *);
	/** if  both colliders participate into a  OnTriggerStay, return true
	* \param _collider
	* \return both colliders participate into a  OnTriggerStay, return true
	*/
	GEAR_API bool OnTriggerStay(Collider *, Collider *);
	/*! ask to every collider if it hits the raycast, obtain and return the collider with the closest intersection time
	* \param _body
	* \return if both are the same.  true
	*/
	GEAR_API   std::pair<Collider * , glm::vec3 > Raycast(Ray * a_ray);

	GEAR_API AABBTree* GetTree();

private:
	GEAR_API PhysicsManager();
	GEAR_API ~PhysicsManager();
	/// singleton accessible everywhere
	static PhysicsManager* mInstance;

private:
	glm::vec3 gravity; /*!< by default all objects will be modified by this gravity */
	Collider *col1;	/*!< collider1 to evaluate*/
	Collider *col2;	/*!< collider2 to eveluate*/
	int impulseIterations = 3;/*!<  randomly decided to stabilize the impulse, make 3 calculations*/
	CollisionManager *mpCollisionManager;/*!< helper class to calculate collisions*/
	AABBTree* dynamicAABBTree;
public:
	//THREADS

	GEAR_API void Thread1(size_t a_Thread_ID);
	GEAR_API void GenerateContacts(Component*, std::vector<Component*>);
 	GEAR_API void Thread2(size_t a_Thread_ID);
 	GEAR_API void updateAccleration1(Component*, float a_fixedDeltaTime);

	std::thread* workerThread[4];
	bool isRunning[4];
	//alignas(64) bool threadFlag[2];
	volatile bool threadFlag[4];
	float fixedDeltaTime;
	std::shared_mutex m_mutex;
};