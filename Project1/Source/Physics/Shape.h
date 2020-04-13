#pragma once

#include "Components/Collider.h"
#include "Component.h"
#include "Components/Transform.h"
#include "Managers/ComponentManager.h"
#include "Editor/RayCast.h"
enum ShpType
{
	_SPHERE,
	_AABB,
	_OBB,
	_CAPSULE,
	_CYLINDER,
	_OTHER2
};


/*! Used on any Physical or Graphical Game Object. 
	It contains the basic Render Parameters to use, from this Base Clase 
	we extend to the basic shape types used
 *  \brief base class to provide all collider with same base shape
 * from this we know half Extents and
 */
class Shape
{
public:
	GEAR_API Shape(ShpType aShapeType, int aOwnerId, Collider* apOwnerCollider);
	GEAR_API Shape(ShpType aShapeType, glm::vec3 size);
	GEAR_API virtual ~Shape();
	GEAR_API void DisplayDebug();
	GEAR_API void Update(float aDeltaTime) {};
	GEAR_API virtual glm::vec3 GetHalfExtents() = 0;

	GEAR_API virtual std::pair<bool, float> Intersected(Ray * a_ray) = 0;
	GEAR_API virtual Shape* Clone() = 0;
	GEAR_API virtual void UpdateSize(glm::vec3 size)= 0;
	GEAR_API virtual void SetAxis(glm::mat3 rotMatrix) = 0;
	GEAR_API virtual float GetSurfaceArea() = 0;
	GEAR_API virtual float GetVolume() = 0;
	GEAR_API virtual glm::vec3 Support(glm::vec3 dir) = 0;
	GEAR_API virtual void Print() = 0;
	GEAR_API virtual void Init() {};
	GEAR_API void Reset();
	GEAR_API virtual void SetCenter(glm::vec3 pos);
private:
	GEAR_API Shape();

public:
	glm::vec3 mCenter;
	glm::mat3 mRot;
	ShpType mType;/*!<what type is the shape*/
	int mOwnerId; /*!<id of owner gameObject*/
	Collider *mpOwnerCollider;/*!<reference to owner's Collider*/
};

/*!
 * \class SHPERE
 *  \brief The Sphere shape for collisions
 */
class SPHERE : public Shape
{
public:
	GEAR_API SPHERE(Collider* a_pOwnerCollider, int a_OwnerId);
	GEAR_API SPHERE(glm::vec3 size);
	GEAR_API ~SPHERE();
	GEAR_API void SetValue(float a_Radius);
	GEAR_API glm::vec3 GetHalfExtents();
	GEAR_API std::pair<bool, float> Intersected(Ray * a_ray);
	GEAR_API virtual SPHERE* Clone();
	GEAR_API virtual void UpdateSize(glm::vec3 size);
	GEAR_API virtual void SetAxis(glm::mat3 rotMatrix);
	GEAR_API virtual float GetSurfaceArea();
	GEAR_API virtual float GetVolume();
	GEAR_API virtual glm::vec3 Support(glm::vec3 dir);
	GEAR_API virtual void Print() {
		std::cout << "mCenter (" << mCenter.x << "," << mCenter.y<< "," << mCenter.z<< ") Radius: " << mRadius<<std::endl;
	}
public:
	float mRadius;/*!<radius of the sphere*/
};

class OBB :public Shape
{
public:
	GEAR_API OBB(Collider* a_pOwnerCollider, int a_OwnerID);
	GEAR_API OBB(glm::vec3 size);
	GEAR_API ~OBB();
	GEAR_API void SetValue(glm::vec3 a_e);
	GEAR_API virtual void UpdateSize(glm::vec3 size);
	GEAR_API virtual void SetAxis(glm::mat3 rotMatrix);
	/** get referent points of the object, based on transforms scale and on specific offset
	* \return Box's  Center Point
	*/
	GEAR_API glm::vec3 GetHalfExtents();
	/** compare shape against ray
	* \return 1 boolean true if interception
	* \return 2 float of the interseption time
	*/
	GEAR_API std::pair<bool, float> Intersected(Ray * a_ray);
	GEAR_API virtual void Init();
	GEAR_API virtual OBB* Clone();
	GEAR_API virtual float GetSurfaceArea();
	GEAR_API virtual float GetVolume();
	GEAR_API virtual glm::vec3 Support(glm::vec3 dir);
	GEAR_API virtual void Print() {
		std::cout << "mCenter (" << mCenter.x << "," << mCenter.y << "," << mCenter.z << ") Extents: " << mE.x << "," << mE.y << "," << mE.z << ")" << std::endl;
	}
public:
	glm::vec3 mE;
};


/*!class AABB
 *  \brief The AABB shape for collisions
 */
class AABB : public Shape
{
public:
	GEAR_API AABB(Collider* a_pOwnerCollider, int a_OwnerID);
	GEAR_API AABB(glm::vec3 size);
	GEAR_API ~AABB();
	GEAR_API void SetValue(float a_Top, float a_Bottom, float a_Left, float a_Right, float a_Front, float a_Back);
	/** get referent points of the object, based on transforms scale and on specific offset
	* \return Box's  Center Point 
	*/
	GEAR_API void Reset();
	GEAR_API glm::vec3 GetHalfExtents();
	/** compare shape against ray 
	* \return 1 boolean true if interception
	* \return 2 float of the interseption time
	*/
	GEAR_API void Union(AABB* &a, AABB* &b);
	GEAR_API bool Contains(Shape* shp);
	GEAR_API virtual void Init();
	GEAR_API std::pair<bool, float> Intersected(Ray * a_ray);
	GEAR_API virtual void UpdateSize(glm::vec3 size);
	GEAR_API virtual void SetAxis(glm::mat3 rotMatrix);
	GEAR_API virtual AABB* Clone();
	GEAR_API virtual float GetSurfaceArea();
	GEAR_API virtual float GetVolume();
	GEAR_API virtual glm::vec3 Support(glm::vec3 dir);

	//GEAR_API static void MakeUnion(AABB*& a, AABB* &b,AABB * result);
	GEAR_API static void MakeBoundingAABB(Shape* &shp, AABB* &result, glm::vec3 padding = glm::vec3(0));
	GEAR_API virtual void Print() {
		std::cout << "mCenter (" << mCenter.x << "," << mCenter.y << "," << mCenter.z << ") Extents: " << mTop << "," << mBottom << "," << mRight << "," << mLeft << ","<< mFront << "," << mBack <<")" << std::endl;
	}
public:
	float mTop;/*!<top of the AABB, Y direction*/
	float mRight;/*!<right of the AABB, X direction*/
	float mBottom;/*!<bottom of the AABB, Y direction*/
	float mLeft;/*!<left of the AABB, X direction*/
	float mFront;/*!<front of the AABB, Z direction*/
	float mBack;/*!<back of the AABB, Z direction*/
	glm::vec3 mE;/*!<extents of the AABB*/
};

/*!
 * \class CAPSULE
 * \brief The Sphere swept shape for collisions y-axis aligned
 * 
 * 
 */
class CAPSULE : public Shape 
{
public:
	GEAR_API CAPSULE(Collider* a_pOwnerCollider, int a_OwnerID);
	GEAR_API CAPSULE(glm::vec3 size);
	/** compare shape against ray
	* \return 1 boolean true if interception
	* \return 2 float of the interseption time
	*/
	GEAR_API virtual std::pair<bool, float> Intersected(Ray * a_ray);
	GEAR_API virtual Shape* Clone();
	GEAR_API virtual void UpdateSize(glm::vec3 size);
	GEAR_API virtual void SetAxis(glm::mat3 rotMatrix);
	GEAR_API virtual float GetSurfaceArea();
	GEAR_API virtual float GetVolume();
	GEAR_API virtual glm::vec3 Support(glm::vec3 dir);
	GEAR_API virtual void Print();
	GEAR_API virtual glm::vec3 GetHalfExtents();
public:
	float mRadius;/*!<radius of the sphere for the capsule tied to half the x value of the scale*/
	float mLength;/*!<length the capsule tied to the y value of the the scale minus 2xmRadius (minimum 0)*/
};

class CYLINDER : public Shape
{
public:
	GEAR_API CYLINDER(Collider* a_pOwnerCollider, int a_OwnerID);
	GEAR_API CYLINDER(glm::vec3 size);
	/** compare shape against ray
	* \return 1 boolean true if interception
	* \return 2 float of the interseption time
	*/
	GEAR_API virtual std::pair<bool, float> Intersected(Ray * a_ray);
	GEAR_API virtual Shape* Clone();
	GEAR_API virtual void UpdateSize(glm::vec3 size);
	GEAR_API virtual void SetAxis(glm::mat3 rotMatrix);
	GEAR_API virtual float GetSurfaceArea();
	GEAR_API virtual float GetVolume();
	GEAR_API virtual glm::vec3 Support(glm::vec3 dir);
	GEAR_API virtual void Print();
	GEAR_API virtual glm::vec3 GetHalfExtents();
public:
	float mRadius;/*!<radius of the sphere for the capsule tied to half the x value of the scale*/
	float mLength;/*!<length the capsule tied to the y value of the the scale minus 2xmRadius (minimum 0)*/
};

