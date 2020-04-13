/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
Author: Ivan Cazares, ivan.c, 60004418
- End Header --------------------------------------------------------*/

#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Shape.h"
#include "Components/Body.h"
#include "Components/Transform.h"

#define COMPONENT_MANAGER ComponentManager::GetInstance()

Shape::Shape(ShpType a_ShapeType, int a_OwnerId, Collider* a_pOwnerCollider)
{
	mType = a_ShapeType;
	mOwnerId = a_OwnerId;
	mpOwnerCollider = a_pOwnerCollider;
}

Shape::Shape(ShpType a_ShapeType, glm::vec3 size)
{
	mType = a_ShapeType;
	mOwnerId = -1;
	mpOwnerCollider = nullptr;
}

GEAR_API void Shape::SetCenter(glm::vec3 pos)
{
	mCenter = pos;
	if (mpOwnerCollider) {
		Body* b = mpOwnerCollider->mpBody;
		if (b) b->mPos = mCenter;
	}
}

GEAR_API void Shape::Reset()
{
	mType = _OTHER2;
	mOwnerId = 0;
	mpOwnerCollider = nullptr;
}

Shape::Shape()
{}

Shape::~Shape()
{}

void Shape::DisplayDebug()
{}


SPHERE::SPHERE(Collider* a_pOwnerCollider, int a_OwnerId) :Shape(_SPHERE, a_OwnerId, a_pOwnerCollider), mRadius(0.5f)
{
	mRadius = 0.5f;
}

SPHERE::SPHERE(glm::vec3 size) : Shape(_SPHERE,size)
{
	mRadius = size.x/2.f;
}

SPHERE::~SPHERE()
{}

void SPHERE::SetValue(float a_Radius)
{
	mRadius = a_Radius;
}

glm::vec3 SPHERE::GetHalfExtents()
{
	glm::vec3 extents;
	extents = glm::vec3(mRadius, mRadius, mRadius);
	return extents;
}

std::pair<bool, float> SPHERE::Intersected(Ray * a_ray)
{
	glm::vec3 p = a_ray->origin;
	glm::vec3 d = a_ray->direction;
	glm::vec3 m = p - mCenter;

	float b = glm::dot(m, d);
	float c = glm::dot(m, m) - (mRadius*mRadius);
	if(c > 0.f && b > 0.f) 
		return std::make_pair(false, 0.0f);

	float discriminate = b * b - c;
	if(discriminate < 0.f)
		return std::make_pair(false, 0.0f);
	float t = -b - sqrtf(discriminate);
	if (t < 0) {
		return std::make_pair(false, 0.0f);
	}

	//ContactPoint = r.origin + r.direction*t
	glm::vec3 contactPoint = a_ray->origin + a_ray->direction* t;

	return  std::make_pair(true, t);
}

GEAR_API SPHERE * SPHERE::Clone()
{
	SPHERE* sph = new SPHERE(glm::vec3(mRadius));
	sph->mCenter = mCenter;
	return sph;
}

GEAR_API void SPHERE::UpdateSize(glm::vec3 size)
{
	mRadius = size.x/2.f;
	glm::mat3 mI = glm::mat3(0);
	glm::mat3 mIInv = glm::mat3(0);
	if (mpOwnerCollider) {
		Body* b = mpOwnerCollider->mpBody;
		if (b) {
			if (b->mMass <= 0) return;
			mI[0][0] = (2.f / 5.f) *  b->mMass *(mRadius*mRadius );
			mI[1][1] = (2.f / 5.f) *  b->mMass *(mRadius*mRadius);
			mI[2][2] = (2.f / 5.f) *  b->mMass *(mRadius*mRadius);
			b->mI_Body = mI;
			b->mInvI_Body = glm::inverse(mI);
		}
	}
}

GEAR_API void SPHERE::SetAxis(glm::mat3 rotMatrix)
{

}

GEAR_API float SPHERE::GetSurfaceArea()
{
	return 4 * mRadius*mRadius*glm::pi<float>();
}

GEAR_API float SPHERE::GetVolume()
{
	return (4.f / 3.f) * mRadius*mRadius*mRadius*glm::pi<float>();
}

GEAR_API glm::vec3 SPHERE::Support(glm::vec3 dir)
{
	return mCenter + mRadius * glm::normalize(dir);
}



AABB::AABB(Collider* a_pOwnerCollider, int a_OwnerID) : Shape(_AABB, a_OwnerID, a_pOwnerCollider)
{
	mOwnerId = a_OwnerID;
	mType = _AABB;
}

AABB::AABB(glm::vec3 size) : Shape(_AABB, size)
{
	mE = size / 2.f;

}

AABB::~AABB()
{
}

void AABB::SetValue(float a_Top, float a_Bottom, float a_Left, float a_Right, float a_Front, float a_Back)
{
	mTop = a_Top;
	mRight = a_Right;
	mBottom = a_Bottom;
	mLeft = a_Left;
	mFront = a_Front;
	mBack = a_Back;
	mE.y = (mTop - mBottom) / 2.f;
	mE.x = (mRight - mLeft) / 2.f;
	mE.z = (mFront - mBack) / 2.f;
	mCenter = glm::vec3(mRight - mE.x, mTop - mE.y, mFront - mE.z);
}

void AABB::Init() {

}

GEAR_API void AABB::MakeBoundingAABB(Shape *& shp, AABB* &result, glm::vec3 padding)
{

	if (shp->mType == ShpType::_SPHERE) {
		SPHERE*  sphere = static_cast<SPHERE*>(shp);
		result->UpdateSize(glm::vec3(sphere->mRadius)*2.f);
		result->mE += padding;
		result->mCenter = sphere->mCenter;
		return;
	}
	else if (shp->mType == ShpType::_AABB) {
		AABB*  aabb = static_cast<AABB*>(shp);
		result = aabb->Clone();
		result->mCenter = aabb->mCenter;
		result->UpdateSize(2.f*(aabb->mE) + padding);
		return;
	}
	else if (shp->mType == ShpType::_OBB) {
		OBB*  obb = static_cast<OBB*>(shp);
		glm::vec3 e = obb->mE;
		glm::vec3 c1 = glm::vec3(e);
		glm::vec3 c2 = glm::vec3(-e.x, e.y, e.z);
		glm::vec3 c3 = glm::vec3(e.x, -e.y, e.z);
		glm::vec3 c4 = glm::vec3(-e.x, -e.y, e.z);
		std::vector<glm::vec3> points = { c1,c2,c3,c4 };//c2,c3,c4,c5,c6,c7,c8
		glm::vec3 maxs = glm::vec3(-1);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 3; ++j) {
				float val = fabsf((obb->mRot*points[i])[j]);//fabsf(points[i][j]);
				if (val > maxs[j]) maxs[j] = val;
			}
		}

		

		result->mCenter = obb->mCenter;
		result->UpdateSize(2.f*maxs + padding);
		return;
	}
	else if (shp->mType == ShpType::_CAPSULE) {
		CAPSULE*  cap = static_cast<CAPSULE*>(shp);
		glm::vec3 size = glm::vec3(cap->mRadius*2.f, cap->mRadius*2.f+cap->mLength, cap->mRadius*2.f);
		result->mCenter = cap->mCenter;
		result->UpdateSize(size + padding);
		//check this
	}
	else if (shp->mType == ShpType::_CYLINDER) {
		CYLINDER*  cyl = static_cast<CYLINDER*>(shp);
		glm::vec3 size = glm::vec3(cyl->mRadius*2.f, cyl->mLength, cyl->mRadius*2.f);
		size = cyl->mRot*size;
		for (int i = 0; i < 3; ++i)
			size[i] = fabsf(size[i]);
		result->mCenter = cyl->mCenter;
		result->UpdateSize(size + padding);
		//check this
	}
}

GEAR_API void AABB::Union(AABB * &a, AABB * &b)
{
	glm::vec3 amaxs = a->mCenter + a->mE;
	glm::vec3 amins = a->mCenter - a->mE;
	glm::vec3 bmaxs = b->mCenter + b->mE;
	glm::vec3 bmins = b->mCenter - b->mE;

	mTop = std::max( amaxs.y, bmaxs.y);
	mBottom = std::min(amins.y, bmins.y);
	mRight = std::max(amaxs.x, bmaxs.x);
	mLeft = std::min(amins.x, bmins.x);
	mFront = std::max(amaxs.z, bmaxs.z);
	mBack = std::min(amins.z, bmins.z);

	mE.y = (mTop - mBottom)/2.f;
	mE.x = (mRight - mLeft)/2.f;
	mE.z = (mFront - mBack)/2.f;

	mCenter = glm::vec3(mRight - mE.x, mTop - mE.y, mFront - mE.z);
}

GEAR_API bool AABB::Contains(Shape * shp)
{
	float l, r, t, b, f, bk;
	
	if (shp->mType == ShpType::_SPHERE) {
		SPHERE*  sphere = static_cast<SPHERE*>(shp);
		b = sphere->mCenter.y - sphere->mRadius;
		t = sphere->mCenter.y + sphere->mRadius;
		l = sphere->mCenter.x - sphere->mRadius;
		r = sphere->mCenter.x + sphere->mRadius;
		bk = sphere->mCenter.z - sphere->mRadius;
		f = sphere->mCenter.z + sphere->mRadius;
	}
	
	if (shp->mType == ShpType::_AABB) {
		AABB*  aabb = static_cast<AABB*>(shp);
		b = aabb->mBottom;
		t = aabb->mTop;
		l = aabb->mLeft;
		r = aabb->mRight;
		f = aabb->mFront;
		bk = aabb->mBack;
	}

	
	if (shp->mType == ShpType::_OBB) {
		OBB*  obb = static_cast<OBB*>(shp);
		glm::vec3 e = obb->mE;

		glm::vec3 c1 = glm::vec3(e);
		glm::vec3 c2 = glm::vec3(-e.x, e.y, e.z);
		glm::vec3 c3 = glm::vec3(e.x, -e.y, e.z);
		glm::vec3 c4 = glm::vec3(-e.x, -e.y, e.z);
		std::vector<glm::vec3> points = { c1,c2,c3,c4 };
		glm::vec3 maxs = glm::vec3(-1);
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 3; ++j) {
				float val = fabsf((obb->mRot*points[i])[j]);
				if (val > maxs[j]) maxs[j] = val;
			}
		}
		b = obb->mCenter.y - maxs.y;
		t = obb->mCenter.y + maxs.y;
		l = obb->mCenter.x - maxs.x;
		r = obb->mCenter.x + maxs.x;
		f = obb->mCenter.z + maxs.z;
		bk = obb->mCenter.z - maxs.z;
	}

	if (shp->mType == ShpType::_CAPSULE) {
		CAPSULE*  cap = static_cast<CAPSULE*>(shp);
		b = cap->mCenter.y - (cap->mLength/2.f+cap->mRadius);
		t = cap->mCenter.y + (cap->mLength / 2.f + cap->mRadius);
		l = cap->mCenter.x - cap->mRadius;
		r = cap->mCenter.x + cap->mRadius;
		bk = cap->mCenter.z - cap->mRadius;
		f = cap->mCenter.z + cap->mRadius;
	}


	return mBottom <= b && mTop >= t &&  
			mLeft <= l && mRight >= r &&
			mBack <= bk && mFront >= f;
}


GEAR_API void AABB::UpdateSize(glm::vec3 size)
{
	mE = size/2.f;
	float h = mE.y;
	mTop = h + mCenter.y;
	mBottom = -h + mCenter.y;
	
	float w =  mE.x;
	mRight = w +mCenter.x;
	mLeft = -w+ mCenter.x;

	float d  = mE.z;
	mFront = d + mCenter.z;
	mBack = -d + mCenter.z;
	
	//*
	glm::mat3 mI = glm::mat3(0);
	glm::mat3 mIInv = glm::mat3(0);
	if (mpOwnerCollider) {
		Body* b = mpOwnerCollider->mpBody;
		if (b) {
			if (b->mMass <= 0) return;
			mI[0][0] = (1.f / 12.f) *  b->mMass *(d*d + h * h);
			mI[1][1] = (1.f / 12.f) *  b->mMass *(d*d + w * w);
			mI[2][2] = (1.f / 12.f) *  b->mMass *(w*w + h * h);
			b->mI_Body = mI;
			b->mInvI_Body = glm::inverse(mI);
		}
	}
}

GEAR_API void AABB::SetAxis(glm::mat3 rotMatrix)
{
	
}

GEAR_API AABB * AABB::Clone()
{
	AABB* result = new AABB(mE);
	result->mCenter = mCenter;
	return result;
}

GEAR_API float AABB::GetSurfaceArea()
{
	return 2.f* (2.f*mE.x) * (2.f*mE.y) + 2.f*(2.f*mE.x)*(2.f*mE.z) + 2.f*(2.f*mE.y)*(2.f*mE.z);
}

GEAR_API float AABB::GetVolume()
{
	return (2.f*mE.x) * (2.f*mE.y)*(2.f*mE.z);
}

GEAR_API glm::vec3 AABB::Support(glm::vec3 dir)
{
	
	dir = glm::normalize(dir);
	
	float w = mE.x;
	float h = mE.y;
	float d = mE.z;
	
	float x = (dir.x > 0) ? w : -w;
	float y = (dir.y > 0) ? h : -h;
	float z = (dir.z > 0) ? d : -d;
	
	if (fabsf(dir.x) <= FLT_EPSILON) x = 0;
	if (fabsf(dir.y) <= FLT_EPSILON) y = 0;
	if (fabsf(dir.z) <= FLT_EPSILON) z = 0;

	glm::vec3 result =glm::vec3(x, y, z);
	glm::vec3 support = result + mCenter;
	//std::cout << "support after translate (" << support.x << "," << support.y << "," << support.z << ")" << std::endl;
	return support; 
}



void AABB::Reset()
{
	mTop = mRight = mBottom = mLeft = mFront = mBack = 0.0f;
	mE = glm::vec3(-1.0f);
}

glm::vec3 AABB::GetHalfExtents() //Box's  Center Point 
{

	return glm::vec3((mRight - mLeft) * 0.5f, (mTop - mBottom) * 0.5f, (mFront - mBack) * 0.5f);//basically is 0
}

/** compare shape against ray
* \return 1 boolean true if interception
* \return 2 float of the interseption time
*/
std::pair<bool, float> AABB::Intersected(Ray * a_ray)
{
	//if ray intersects on 3 planes, having tmin < tmax.. then collision is true, and returns tMin. 

	glm::vec3 o = a_ray->origin;
	glm::vec3 d = a_ray->direction;

	float minTx, maxTx, minTy, maxTy, minTz, maxTz;

	minTx = (mLeft - o.x) / d.x;
	maxTx = (mRight - o.x) / d.x;
	if (minTx > maxTx) { float tempx = minTx; minTx = maxTx; maxTx = tempx; }

	minTy = (mTop - o.y) / d.y;
	maxTy = (mBottom - o.y) / d.y;
	if (minTy > maxTy) { float tempy = minTy; minTy = maxTy; maxTy = tempy; }

	minTz = (mFront - o.z) / d.z;
	maxTz = (mBack - o.z) / d.z;
	if (minTz > maxTz) { float tempz = minTz; minTz = maxTz; maxTz = tempz; }

	float tMin = glm::max(glm::max(minTx, minTy), minTz);
	float tMax = glm::min(glm::min(maxTx, maxTy), maxTz);

	if (tMin > tMax || tMin < 0) {
		return std::make_pair(false, 0.0f);
	}

	//ContactPoint = r.origin + r.direction*t
	glm::vec3 contactPoint = a_ray->origin + a_ray->direction* tMin;
	return  std::make_pair(true, tMin);

}





OBB::OBB(Collider * a_pOwnerCollider, int a_OwnerID) : Shape(_OBB, a_OwnerID, a_pOwnerCollider)
{
	mpOwnerCollider = a_pOwnerCollider;
	mOwnerId = a_OwnerID;
}

OBB::OBB(glm::vec3 size): Shape(_OBB,size)
{
	mE = size/2.f;
}

OBB::~OBB()
{
}

GEAR_API void OBB::SetValue(glm::vec3 a_e)
{
	mE = a_e;
}

GEAR_API void OBB::UpdateSize(glm::vec3 size)
{
	mE = size/2.f;
	float d = size.z; float w = size.x;  float h = size.y;
	glm::mat3 I = glm::mat3(0);
	glm::mat3 IInv = glm::mat3(0);
	if (mpOwnerCollider) {
		Body* b = mpOwnerCollider->mpBody;
		if (b) {
			if(b->mMass <= 0) return;


			I[0][0] = (1.f / 12.f) *  b->mMass *(d*d + h * h);
			I[1][1] = (1.f / 12.f) *  b->mMass *(d*d + w * w);
			I[2][2] = (1.f / 12.f) *  b->mMass *(w*w + h * h);
			b->mI_Body = I;
			b->mInvI_Body = glm::inverse(I);
		}
	}
}

GEAR_API void OBB::SetAxis(glm::mat3 rotMatrix)
{
	mRot = rotMatrix;
}


GEAR_API glm::vec3 OBB::GetHalfExtents()
{
	return mE;
}

GEAR_API std::pair<bool, float> OBB::Intersected(Ray * a_ray)
{
	glm::mat3 i_rot = glm::inverse(mRot);
	glm::vec3 o = i_rot * a_ray->origin;
	glm::vec3 d = i_rot * a_ray->direction;
	float minTx, maxTx, minTy, maxTy, minTz, maxTz;
	glm::vec3 mins = -mE;
	glm::vec3 maxs =  mE;
	minTx = (mins.x - o.x) / d.x;
	maxTx = (maxs.x - o.x) / d.x;
	if (minTx > maxTx) { float tempx = minTx; minTx = maxTx; maxTx = tempx; }

	minTy = (maxs.y - o.y) / d.y;
	maxTy = (mins.y - o.y) / d.y;
	if (minTy > maxTy) { float tempy = minTy; minTy = maxTy; maxTy = tempy; }

	minTz = (maxs.z - o.z) / d.z;
	maxTz = (mins.z - o.z) / d.z;
	if (minTz > maxTz) { float tempz = minTz; minTz = maxTz; maxTz = tempz; }

	float tMin = glm::max(glm::max(minTx, minTy), minTz);
	float tMax = glm::min(glm::min(maxTx, maxTy), maxTz);

	if (tMin > tMax || tMin < 0) {
		return std::make_pair(false, 0.0f);
	}

	//ContactPoint = r.origin + r.direction*t
	glm::vec3 contactPoint = (o + d* tMin)*mRot+mCenter;
	return  std::make_pair(true, tMin);
}

GEAR_API void OBB::Init()
{
	SetAxis(mpOwnerCollider->mpTransform->GetRotationMatrix());
	mCenter = mpOwnerCollider->mpTransform->GetPosition();
}

GEAR_API OBB * OBB::Clone()
{
	OBB* obb = new OBB(mE*2.f);
	obb->SetAxis(mRot);
	return obb;
}

GEAR_API float OBB::GetSurfaceArea()
{
	return  2.f* (2.f*mE.x) * (2.f*mE.y) + 2.f*(2.f*mE.x)*(2.f*mE.z) + 2.f*(2.f*mE.y)*(2.f*mE.z);
}

GEAR_API float OBB::GetVolume()
{
	return (2.f*mE.x)*(2.f*mE.y)*(2.f*mE.z);
}

GEAR_API glm::vec3 OBB::Support(glm::vec3 dir)
{
	//std::cout << "direction (" << dir.x << "," << dir.y << "," << dir.z << ")" << std::endl;

	glm::mat3 i_rot = glm::inverse(mRot);
	
	glm::vec3 rot_d = i_rot *dir;
	float w = mE.x;
	float h = mE.y;
	float d = mE.z;
	
	float x = (rot_d.x > 0) ? w : -w;
	float y = (rot_d.y > 0) ? h : -h;
	float z = (rot_d.z > 0) ? d : -d;
	if (fabsf(rot_d.x) <= FLT_EPSILON) x = 0;
	if (fabsf(rot_d.y) <= FLT_EPSILON) y = 0;
	if (fabsf(rot_d.z) <= FLT_EPSILON) z = 0;
	glm::vec3 tmp = glm::vec3(x,y,z);
	//std::cout << "support before all (" << tmp.x << "," << tmp.y << "," << tmp.z << ")" << std::endl;
	glm::vec3 r_tmp =  mRot*tmp;
	//std::cout << "support after rot befor trans (" << r_tmp.x << "," << r_tmp.y << "," << r_tmp.z << ")" << std::endl;
	//std::cout << "+ trans (" << mCenter.x << "," << mCenter.y << "," << mCenter.z << ")" << std::endl;
	glm::vec3 result = r_tmp + mCenter;
	//std::cout << "support after translate (" << result.x << "," << result.y << "," << result.z << ")" << std::endl;

	return result;
}

CAPSULE::CAPSULE(Collider * a_pOwnerCollider, int a_OwnerID) : Shape(_CAPSULE, a_OwnerID, a_pOwnerCollider)
{
	
}

CAPSULE::CAPSULE(glm::vec3 size) :Shape(_CAPSULE,size)
{
}

GEAR_API std::pair<bool, float> CAPSULE::Intersected(Ray * a_ray)
{
	return std::pair<bool, float>();
}

GEAR_API Shape * CAPSULE::Clone()
{
	CAPSULE* cap = new CAPSULE(glm::vec3(0));
	cap->mRadius = mRadius;
	cap->mLength = mLength;
	cap->mRot = mRot;
	return cap;
}

GEAR_API void CAPSULE::UpdateSize(glm::vec3 size)
{
	mRadius = (size.x / 2.f);
	mLength = size.y - (2.f * mRadius);
	if (mLength < 0) mLength = 0;
	if (mpOwnerCollider) {
		glm::mat3 I = glm::mat3(0);
		glm::mat3 J = glm::mat3(0);
		glm::mat3 IInv = glm::mat3(0);
		Body* b = mpOwnerCollider->mpBody;
		if (b) {
			if (b->mMass <= 0) return;
			glm::mat3 I = glm::mat3(0);
			glm::mat3 J_cyl = glm::mat3(0);
			glm::mat3 IInv = glm::mat3(0);

			J[0][0] = b->mMass *((3 * mRadius + 2 * mLength) / 8.f) * mLength;
			J[1][1] = (2.f / 5.f) * b->mMass * mRadius*mRadius;
			J[2][2] = J[0][0];
			J_cyl[0][0] = (1.f / 12.f) *  b->mMass *(3* (mRadius * mRadius) + mLength * mLength);
			J_cyl[1][1] = (1.f / 2.f) *  b->mMass *(mRadius * mRadius);//y is the height 
			J_cyl[2][2] = J_cyl[0][0];
			I = J + J_cyl;
			b->mI_Body = I;
			b->mInvI_Body = glm::inverse(I);
		}
	}
}

GEAR_API void CAPSULE::SetAxis(glm::mat3 rotMatrix)
{
	
}

GEAR_API float CAPSULE::GetSurfaceArea()
{
	return 2.f*glm::pi<float>()*mRadius*(2.f*mRadius + mLength);// 2pir(2r + a)
}

GEAR_API float CAPSULE::GetVolume()
{ 
	return glm::pi<float>()*(mRadius*mRadius)*((4.f/3.f)*mRadius +mLength);//pi*r2((4/3)r + a)
}

GEAR_API glm::vec3 CAPSULE::Support(glm::vec3 dir)
{
	glm::vec3 dir_xz = glm::vec3(dir.x, 0, dir.z);
	glm::vec3 result = glm::normalize(dir_xz)*mRadius;
	result.y = (dir.y > 0) ? mLength / 2.f : -mLength / 2.f;
	return result + mCenter;
}

GEAR_API void CAPSULE::Print()
{
	std::cout << "Length " << mLength << " radius " << mRadius;
	std::cout << " mCenter (" << mCenter.x << "," << mCenter.y << "," << mCenter.z << ")" << std::endl;
}

GEAR_API glm::vec3 CAPSULE::GetHalfExtents()
{
	return glm::vec3(0);
}

CYLINDER::CYLINDER(Collider * a_pOwnerCollider, int a_OwnerID) : Shape(_CYLINDER,a_OwnerID,a_pOwnerCollider)
{
}

CYLINDER::CYLINDER(glm::vec3 size) :Shape(_CYLINDER,size)
{
	mLength = size.y;
	mRadius = size.x / 2.f;
}

GEAR_API std::pair<bool, float> CYLINDER::Intersected(Ray * a_ray)
{
	glm::mat3 i_rot = glm::inverse(mRot);
	glm::vec3 a = i_rot * a_ray->origin;
	glm::vec3 d = i_rot * a_ray->direction;
	glm::vec3 q = glm::vec3(0,mLength / 2.f,0)+mCenter;
	glm::vec3 p = glm::vec3(0, -mLength / 2.f, 0)+mCenter;

	glm::vec3 m = a - p;
	glm::vec3 n = d;

	float md = glm::dot(m, d);
	float nd = glm::dot(n, d);
	float dd = glm::dot(d, d);

	if(md < 0.f && md+nd <0.f) 
		return std::make_pair(false, 0.0f);

	if (md > dd && md + nd > dd)
		return std::make_pair(false, 0.0f);

	float nn = glm::dot(n, n);
	float mn = glm::dot(m, n);
	float da = dd * nn - nd * nd;
	float k = glm::dot(m, m) - mRadius * mRadius;
	float c = dd * k - md * md;
	float t = 0.f;
	if (fabsf(da) < FLT_EPSILON) {
		if(c>0.f)
			return std::make_pair(false, 0.0f);
		if (md < 0.f)
			t = -mn / nn;
		else if (md > dd)
			t = (nd - mn) / nn;
		else
			t = 0.f;
		return std::make_pair(true, t);
	}
	float b = dd * mn - nd * md;
	float discr = b * b - da * c;
	if(discr < 0.f)
		return std::make_pair(false, 0.0f);
	t = (-b - sqrtf(discr)) / da;
	if(t<0.f || t >1.f)
		return std::make_pair(false, 0.0f);
	if (md + t * nd < 0.f) {
		if(nd <= 0.f)
			return std::make_pair(false, 0.0f);
		t = -md / nd;
	}
	else if (md + t * nd > dd) 
	{
		if (nd >= 0.f)
			return std::make_pair(false, 0.0f);
		t = (dd - md) / nd;
		bool result = (k + dd - 2 * md + t * (2 * (mn - nd) + t * nn)) <= 0.f;
		return std::make_pair(result, t);
	}
	
	return std::make_pair(true, t);
}

GEAR_API Shape * CYLINDER::Clone()
{
	CYLINDER * cly = new CYLINDER(glm::vec3(0));
	cly->mCenter = mCenter;
	cly->mRadius = mRadius;
	cly->mLength = mLength;
	cly->SetAxis(mRot);
	return cly;
}

GEAR_API void CYLINDER::UpdateSize(glm::vec3 size)
{
	mRadius = (size.x / 2.f);
	mLength = size.y;
	if (mLength < 0) mLength = 0;
	if (mpOwnerCollider) {
		Body* b = mpOwnerCollider->mpBody;
		if (b) {
			if (b->mMass <= 0) return;
			glm::mat3 I = glm::mat3(0);
			glm::mat3 J_cyl = glm::mat3(0);
			glm::mat3 IInv = glm::mat3(0);
			J_cyl[0][0] = (1.f / 12.f) *  b->mMass *(3 * (mRadius * mRadius) + mLength * mLength);
			J_cyl[1][1] = (1.f / 2.f) *  b->mMass *(mRadius * mRadius);//y is the height 
			J_cyl[2][2] = J_cyl[0][0];
			I = J_cyl;
			b->mI_Body = I;
			b->mInvI_Body = glm::inverse(I);
		}
	}
}

GEAR_API void CYLINDER::SetAxis(glm::mat3 rotMatrix)
{
	mRot = rotMatrix;
}

GEAR_API float CYLINDER::GetSurfaceArea()
{
	return 2.f*glm::pi<float>()*mRadius*(mLength+mRadius);//pi*2r(h+r)
}

GEAR_API float CYLINDER::GetVolume()
{
	return glm::pi<float>()*mRadius*mRadius*mLength;//pi*r2h
}

GEAR_API glm::vec3 CYLINDER::Support(glm::vec3 dir)
{
	glm::mat3 i_rot = glm::inverse(mRot);

	glm::vec3 rot_d = i_rot * dir;
	
	glm::vec3 dir_xz = glm::vec3(dir.x, 0, dir.z);
	glm::vec3 result = glm::normalize(dir_xz)*mRadius;
	result.y = (dir.y > 0) ? mLength/2.f : -mLength/2.f;

	return mRot * result + mCenter; //convert support to world space
}

GEAR_API void CYLINDER::Print()
{
	std::cout << "Length " << mLength << " radius " << mRadius;
	std::cout << " mCenter (" << mCenter.x << "," << mCenter.y << "," << mCenter.z << ")" << std::endl;
}

GEAR_API glm::vec3 CYLINDER::GetHalfExtents()
{
	return glm::vec3(0);
}
