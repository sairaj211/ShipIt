#pragma once
#include "Components/Collider.h"
#include "Managers/ComponentManager.h" //to traverse on ComponentMap
#include "Components/Body.h"
#include "Component.h"
#include "Shape.h"
enum ContactType
{
	OnCollisionEnter = 0,
	OnCollisionStay,
	OnCollisionExit,
	OnTriggerEnter,
	OnTriggerStay,
	OnTriggerExit,
	NonCollision,
	Other
};

/*! \class Contact
 *  \brief Manages all collisions, generates Forces, corrects Positions
 *
 * Helps PhysicsManager, calculating if objects collide, how far are they, how to react,
 * this generates a list of Collisions, described on ContactPoints, helped by the class Contact2D
 * after, traverse on all Collisions, and generate repelion forces for every tupla
 * also defines specific  behaviors resultant of specific Tupla of Elements, like sparcles, messages, score, healt change
 */
class Contact
{
public:
	/// Access an object value by name, create a null member if it does not exist.
	/// \param key may contain embedded nulls.
	///https://en.wikipedia.org/wiki/Impulse_(physics)
	///http://zonalandeducation.com/mstm/physics/mechanics/momentum/introductoryProblems/momentumSummary2.html
	///units Newtons / second
	GEAR_API Contact(Collider*, Collider*);
	/**   As a rseult of collision between colliders, there is a Applied Force that must modify the repulsion Velocity */
	GEAR_API bool ApplyImpulse(float);
	/**  as the object collides with floor, it will decrease its velocity until stop */
	GEAR_API void ApplyFrictionImpulse(float);
	/**  As a result of non Discretes deltas, on next frame the objects can superpose... correct this offset so they will respect own spaces */
	GEAR_API void CorrectPenetration();
	/**  ask if the contact is OnTrigger enter/stay/exit */
	bool isTrigger();
private:
	GEAR_API glm::vec3 GetRelativeVelocity(Collider* col);
public:
	ContactType mContactType;/*!< Specify if is Collision/Trigger  enter/stay/exit*/
	glm::vec3 mNormal;/*!< in wich direction to react */
	glm::vec3 colPoint;/*!<  point of collision */
	Body * mpBodyMain;/*!<  Main body integrant in the contact */
	Body * mpBodyOther;/*!<  Other body integrant in the contact*/
	Collider* mpColliderMain;/*!< Main collider */
	Collider* mpColliderOther;/*!< Other collider */
	float penetrationDepth;/*!< after penetrating, know far have the objects been penetrated , to apply corrections */
	float mMinCoefRestitution;/*!< ratio of  final to initial relative velocity of 2 objects, from 0 to 1 */
	float mCoeficientStaticFriction;/*!< challenge to defeat before starting movement*/
	float mCoeficientDynamicFriction;/*!< friction of 2 object moving*/

	//local variables to handle collisions
	 
};

/*! \class CollisionManager
 *  \brief Manages all collisions2D, generates Forces, corrects Positions
 *
 * Helps PhysicsManager, calculating if objects collide, how far are they, how to react,
 * this generates a list of Collisions, described on ContactPoints, and information related with the collision helped by the class Contact
 * after physical reactions, it defines specifically reactions to specific Tupla of collision Elements
 */
class CollisionManager
{
public:
	GEAR_API CollisionManager();
	GEAR_API ~CollisionManager();

	GEAR_API Collider * RetrieveFirstContact(Collider *, std::string mlType, ContactType);


	/** Remove all Contacts from the list and clean the structures
	*/
	GEAR_API void ResetContacts();

	/** Compare Actual and Prev lists of contacts, to define  Collision/ Trigger - enter/stay/exit
	*/
	GEAR_API void IdentifyContactTypes();

	/** if both candidate types are designed to collide. return true
	* \param _type0  first  label
	* \param _type1  second label
	* \return if they are designed to collide return true
	*/
	GEAR_API bool CheckCompatibility(std::string a_type0, std::string a_type1);

	/** if type of objects are meant to collide, return true, (also calculate order or reception)
	* \param first  candidate string 1,
	* \param second candidate string 2,
	* \param _type0  designed dupla collition, element 1
	* \param _type1 designed dupla collition, element 2
	* \return
	*/
	GEAR_API bool Colliding(std::string a_first, std::string a_second, std::string a_type0, std::string a_type1);

	/** if type of objects are meant to collide, return true, (also calculate order or reception on a boolean mInOrder)
	* \param first candidate
	* \param second canididate
	* \return if they are meant to contact
	*/
	GEAR_API bool Colliding(std::string a_first, std::string a_second);

	/** first  check if colliders are designed to collide,
	* \then if their collider/trigger relationship is meant to generate a contact,
	* \then if their shapes are intercepting,
	* \if all this is true, then generate a contact point
	* \param col0 candidate collider
	* \param col1 candidate collider
	*/
	GEAR_API void CheckContact(Collider* a_col0, Collider* a_col1);

	/** DEPRECATED:  if specific contact collision/trigger enter/exit/stay is detected, do. specific. things
	*/
	GEAR_API void SolveContactsReactions();

	/**  for all contacts that are Collisions, generate repelion forces
	*/
	GEAR_API void ApplyImpulses(float dt);

	/**  if bodies have constrains on velocities, reset to 0 the specific velocity axis
	*/
	GEAR_API void ConstrainVelocity();

	/** if objects have gone inside others , tries to relocate objects to a safer position behind.
	*/
	GEAR_API void CorrectPenetration();

	/**  Polymorphic method, to solve collisions , according to specific shape combinations
	* \param pShape1 first shape to check
	* \param Pos1 position of first shape
	* \param pShape2 second shape to check
	* \param Pos2 position of second shape
	* \param _type candidate contactType
	* \return ,if it generates a contact point ,, true
	*/
	GEAR_API bool CheckCollisionAndGenerateContact(Shape* pShape1, Shape* pShape2, ContactType _type);

	GEAR_API bool CheckCollision(Shape* pShape1, Shape* pShape2, ContactType _type);

	/** if both contacts are the same .. return true
	* \param  c1 contact caniddate
	* \param  c1 contact candidate
	* \return if both contact have the same colliders.  true
	*/
	GEAR_API bool CompareContacts(Contact * c1, Contact * c2);

	/** polymorphic function, according to shape combination,  use a define CollisionCheckFunction
	* \param pShape1 first shape to check
	* \param Pos1 position of first shape
	* \param pShape2 second shape to check
	* \param Pos2 position of second shape
	* \mContacts  references to the already  generated Contact list, to add a cont
	* \param _type candidate contactType
	*/
	bool(*CollisionFunctions[ShpType::_OTHER2][ShpType::_OTHER2])
		(Shape* pShape1, Shape* pShape2, std::list<Contact*>& mContacts, ContactType, bool generate);

	/** if  this collider participates into a  CollisionEnter, return true
	* \param _collider
	* \return if this collider participates into a  CollisionEnter, return true
	*/
	GEAR_API bool OnCollisionEnter(Collider *);
	/** if  this collider participates into a  CollisionExit, return true
	* \param _collider
	* \return if this collider participates into a  CollisionExit, return true
	*/
	GEAR_API bool OnCollisionExit(Collider *);
	/** if  this collider participates into a  CollisionStay, return true
	* \param _collider
	* \return if this collider participates into a  CollisionStay, return true
	*/
	GEAR_API bool OnCollisionStay(Collider *);
	/** if  this collider participates into a  TriggerEnter, return true
	* \param _collider
	* \return if this collider participates into a  TriggerEnter, return true
	*/
	GEAR_API bool OnTriggerEnter(Collider *);
	/** if  this collider participates into a  TriggerExi, return true
	* \param _collider
	* \return if this collider participates into a  TriggerExi, return true
	*/
	GEAR_API bool OnTriggerExit(Collider *);
	/** if  this collider participates into a  TriggerStay, return true
	* \param _collider
	* \return if this collider participates into a  TriggerStay, return true
	*/
	GEAR_API bool OnTriggerStay(Collider *);
	/*! if the body is implied into a contact of type defined TRUE
	 * \param _body
	 * \param _contactType
	 * \return if both are the same.  true
	 */
	GEAR_API bool CheckCollision(Collider * _body, ContactType _contactType);

	/** if  this collider participates into a  CollisionEnter, return true
	* \param _collider
	* \return if this collider participates into a  CollisionEnter, return true
	*/
	GEAR_API bool OnCollisionEnter(Collider *, Collider *);
	/** if  this collider participates into a  CollisionExit, return true
	* \param _collider
	* \return if this collider participates into a  CollisionExit, return true
	*/
	GEAR_API bool OnCollisionExit(Collider *, Collider *);
	/** if  this collider participates into a  CollisionStay, return true
	* \param _collider
	* \return if this collider participates into a  CollisionStay, return true
	*/
	GEAR_API bool OnCollisionStay(Collider *, Collider *);
	/** if  this collider participates into a  TriggerEnter, return true
	* \param _collider
	* \return if this collider participates into a  TriggerEnter, return true
	*/
	GEAR_API bool OnTriggerEnter(Collider *, Collider *);
	/** if  this collider participates into a  TriggerExi, return true
	* \param _collider
	* \return if this collider participates into a  TriggerExi, return true
	*/
	GEAR_API bool OnTriggerExit(Collider *, Collider *);
	/** if  this collider participates into a  TriggerStay, return true
	* \param _collider
	* \return if this collider participates into a  TriggerStay, return true
	*/
	GEAR_API bool OnTriggerStay(Collider *, Collider *);
	/*! if the body is implied into a contact of type defined TRUE
	 * \param _body
	 * \param _contactType
	 * \return if both are the same.  true
	 */

	 /*! if the body is implied into a contact of type defined TRUE
	  * \param _body
	  * \param _contactType
	  * \return if both are the same.  true
	  */
	GEAR_API bool CheckCollision(Collider * _body1, Collider * _body2, ContactType _contactType);


public:
	std::string mType0, mType1;/*!< recieve two variables to see if them are contemplated. // if parallel programming  thes must change */
	bool mInOrder;/*!<  to avoid doubled comparsion. check if paramether come in order */

	std::list<Contact*> mActualContacts;/*!< contacts actually detected  */
	std::list<Contact*> mPrevContacts;	/*!< contacts of previous frame */
	std::list<Contact*> mContacts;	/*!< MAIN list of Collision/Trigger Enter/Stay/Exit */
private:
	int impulseIterations = 100;
	std::list<std::pair<Collider*, Collider>> mPotentialContacts;
};