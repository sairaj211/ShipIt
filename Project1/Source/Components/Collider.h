/*  -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
Author: Ivan Cazares 60004418
- End Header --------------------------------------------------------*/
#pragma once
#include "Component.h"
#include "Managers/ComponentManager.h"
#include "Transform.h"

class Shape;
class Body;
class Transform;

/*! 
 *  \class Collider
 *  \brief Collider presents 3 friction  coeficients, 4 variables
 *  it has variables related to the physics friction, if is trigger,
 *  what shape is related. to apply collisions
 *  a label mLShape. for identify the type of object, this for explicetly reduce number of calculations
 */

class Collider : public Component
{
public:
	GEAR_API Collider();
	GEAR_API ~Collider();
	GEAR_API void Init();
	GEAR_API void Reset();
	GEAR_API void Initialize(std::string);
	GEAR_API void SelfDelete();
	GEAR_API Component* Create();
	GEAR_API void DisplayDebug();
	GEAR_API void Update(float);
	/*!
	 * Call this to read all of the values of the collider
	 * to the json indicated with the input
	 * \param the root for reading the component
	*/
	GEAR_API void Serialize(Json::Value);
	/*!
	 * Call this to write all of the values of the collider
	 * to the json indicated with the input
	 * \param the root for writing
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	*/
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	GEAR_API virtual void HandleEvent(Event* a_pEvent);
	GEAR_API inline bool IsCollided() { return mIsCollided; }

	GEAR_API inline int GetOwner() { return GetOwnerID(); }
	GEAR_API void Clone(int objID);

public:
	Transform* mpTransform;
	Body * mpBody;
	Shape * mpShape;/*!< AABB,Sphere,OBB */
	std::string mLType; /*!<Label to Collider Objects. REDUCES A LOT THE collission detection calculation. */
	std::string mShape;/*!<label to generate a pointer to related shape*/
	
	float mCoeffRestitution;/*!<doxygen description of the member*/
	float mCoeffStaticFriction;/*!<doxygen description of the member*/
	float mCoeffDynamicFriction;/*!<doxygen description of the member*/
	bool mIsTrigger;/*!<if true, no forces aplies to it*/
	bool mIsCollided;/*!<variable for Graphics. to know if it changes the colliders line to Red*/
	glm::vec3 prevScale;/*!<helping variable to reacalculate the peripherical offsets, for collisions and graphics.*/
	float mWidth, mHeight, mDepth, mRadius;	
	glm::vec3 mOffset;/*!<if the Model Root is not at the center of the position, this is a manual offset to  fix it*/
	glm::vec3 mRescale;/*!<if   Model Scale doesnt match default 1x1x1, this can fit collider it*/
	int mNodeIndex;
};