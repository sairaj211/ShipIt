#include "pch.h"
#include "CollisionManager.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Engine.h"
#include "Events/ContactEvent.h"
#include "Managers/MemoryManager.h"
#include "Timer.h"
#include "Managers/EventManager.h"
#include "GJK.h"

Contact::Contact(Collider * a_c0, Collider * a_c1) {
	mpColliderMain = a_c0;
	mpColliderOther = a_c1;
	mpBodyMain = COMPONENT_MANAGER->GetComponent<Body>(a_c0->GetOwnerID(), "Body");
	mpBodyOther = COMPONENT_MANAGER->GetComponent<Body>(a_c1->GetOwnerID(), "Body");

	// frictionCoeficients
	mMinCoefRestitution = std::fmin(mpColliderMain->mCoeffRestitution, mpColliderOther->mCoeffRestitution);
	mCoeficientStaticFriction = (mpColliderMain->mCoeffStaticFriction + mpColliderOther->mCoeffStaticFriction) * 0.5f;
	mCoeficientDynamicFriction = (mpColliderMain->mCoeffDynamicFriction + mpColliderOther->mCoeffDynamicFriction) * 0.5f;

	penetrationDepth = 0.0f;
	mNormal = glm::vec3(0.0f, 0.0f, 0.0f);
}

bool Contact::ApplyImpulse(float dt)
{
	glm::vec3 pADot = GetRelativeVelocity(mpColliderOther);
	glm::vec3 pBDot = GetRelativeVelocity(mpColliderMain);
	float relVN = glm::dot(mNormal,(pADot - pBDot));
	if (relVN > 0.001f) //seperating
		return false;
	//if (relVN > -0.001f)//resting
		//return;

	glm::vec3 relativeVelocity = pADot - pBDot;//mpBodyOther->mVel - mpBodyMain->mVel;

	float velNormal = relVN;//glm::dot(relativeVelocity, mNormal);

	if ((isnan(relativeVelocity.x) || isnan(relativeVelocity.y) || isnan(relativeVelocity.z)) ||
		(isnan(mNormal.x) || isnan(mNormal.y) || isnan(mNormal.z))) {
		LOG_ERROR("NAN cuaght in collision normal or relative Velocity normal");
		std::cout << "Colliders " << mpBodyMain->GetOwnerID() << " - " << mpBodyOther->GetOwnerID() << std::endl;
		return false;
	}
	
	/*
	if (velNormal > 0) {

		return;
	}*/

	float term1, term2, term3, term4;
	float numerator = -(1.0f + mMinCoefRestitution) * velNormal;
	if (mpBodyMain->mInvMass == 0 && mpBodyOther->mInvMass == 0) return false;
	term1 = mpBodyMain->mInvMass,
	term2 = mpBodyOther->mInvMass,
	term3 = 0.f, 
	term4 = 0.f;
	float j = numerator/
		(term1+term2+term3+term4);
	glm::vec3 impulseVec = j * mNormal;
	if (isnan(j))
		return false;
	mpBodyMain->AddVelocities(-impulseVec, colPoint,true);
	
	mpBodyOther->AddVelocities(impulseVec, colPoint,true);

	if (mpBodyMain->mApplyFriction && mpBodyOther->mApplyFriction)
		ApplyFrictionImpulse(j);
	return true;
}

void Contact::ApplyFrictionImpulse(float a_impulse)
{
	glm::vec3 relativeVelocity = mpBodyOther->mVel - mpBodyMain->mVel;

	float relativeVelNormal = glm::dot(relativeVelocity, mNormal);
	glm::vec3 tangent = glm::normalize(relativeVelocity - (mNormal * relativeVelNormal));

	if (isnan(tangent.x) || isnan(tangent.y) || isnan(tangent.z) || (isinf(tangent.x) || isinf(tangent.y) || isinf(tangent.z)))
		return;

	float fricImpulseMagnitude = -glm::dot(relativeVelocity, tangent) /
		(mpBodyMain->mInvMass + mpBodyOther->mInvMass);

	if (std::fabsf(fricImpulseMagnitude) <= FLT_EPSILON)
		return;

	// Coulomb's law of friction
	glm::vec3 frictionImpulse = abs(fricImpulseMagnitude) < a_impulse * mCoeficientStaticFriction ?
		fricImpulseMagnitude * tangent :
		-a_impulse * tangent * mCoeficientDynamicFriction;
	frictionImpulse.y = 0.0f;
	mpBodyMain->AddVelocities(-frictionImpulse, colPoint,true);
	mpBodyOther->AddVelocities(frictionImpulse , colPoint,true);

}

void Contact::CorrectPenetration()
{
	///http://box2d.org/files/GDC2005/IterativeDynamics.pdf
	float slop = 0.05f; // bigger values must be corrected
	float percent = 0.9f; //dolving this deepth porcentage
	glm::vec3 correction = (std::max(penetrationDepth - slop, 0.0f) /
		(mpBodyMain->mInvMass + mpBodyOther->mInvMass)) * percent * mNormal;
	if ((isnan(correction.x) || isnan(correction.y) || isnan(correction.z))) {
		LOG_ERROR("NAN cuaght in corretion vector during correct penetration");
		std::cout << "Colliders " << mpBodyMain->GetOwnerID() << " - " << mpBodyOther->GetOwnerID() << std::endl;
	}
	mpBodyMain->mPos -= correction * mpBodyMain->mInvMass;
	mpBodyOther->mPos += correction * mpBodyOther->mInvMass;
}

bool CollisionManager::CompareContacts(Contact * a_c1, Contact * a_c2)
{
	if ((a_c1->mpColliderMain == a_c2->mpColliderMain && a_c1->mpColliderOther == a_c2->mpColliderOther))
		return true;

	return false;
}

// OnCollisionEnter, onCollisionExit, OnCollisionStay
void CollisionManager::IdentifyContactTypes()
{
	//Compare Actual list with Previous to find Enter and Stay
	for (Contact * actualContact : mActualContacts)
	{
		bool foundInPrev = false;
		for (Contact * prevContact : mPrevContacts)
			if (CompareContacts(actualContact, prevContact)) {
				foundInPrev = true;
				break;
			}

		actualContact->mContactType =
			foundInPrev ?
			actualContact->mContactType == ContactType::OnCollisionEnter ? ContactType::OnCollisionStay : ContactType::OnTriggerStay :
			actualContact->mContactType;
		mContacts.push_back(actualContact);
	}
	//Compare Actual list with Previous to find Exit
	for (Contact * prevContact : mPrevContacts)
	{
		if (prevContact->mpColliderMain->mpShape == nullptr || prevContact->mpColliderOther->mpShape == nullptr)//check for deleted shapes form level changes
			continue;
		bool foundInActual = false;
		for (Contact * actualContact : mActualContacts)
			if (CompareContacts(actualContact, prevContact)) {
				foundInActual = true;
				break;
			}

		if (!foundInActual) {
			
			prevContact->mContactType =
				prevContact->mContactType == ContactType::OnCollisionEnter || prevContact->mContactType == ContactType::OnCollisionStay ?
				ContactType::OnCollisionExit :
				ContactType::OnTriggerExit;
			mContacts.push_back(prevContact);
		}
	}
}


/**   In which axis  is the predominant Normal of collision
* \param_separation
* \param _deltaPosition
*/
glm::vec3 CollisionNormal(glm::vec3 a_separation, glm::vec3 a_deltaPosition)
{
	return
		a_separation.x > a_separation.y ?
		a_separation.x > a_separation.z ?
		glm::vec3(a_deltaPosition.x < 0 ? -1 : 1, 0, 0) ://X
		glm::vec3(0, 0, a_deltaPosition.z < 0 ? -1 : 1) :// Z
		a_separation.z > a_separation.y ?
		glm::vec3(0, 0, a_deltaPosition.z < 0 ? -1 : 1) ://Z
		glm::vec3(0, a_deltaPosition.y < 0 ? -1 : 1, 0);//Y
}

/**  How much the objects are one inside the other.
* \param_separation
* \param _deltaPosition
*/
float PenetrationDepth(glm::vec3 a_separation)
{
	return
		a_separation.x < a_separation.y ?
		a_separation.x < a_separation.z ?
		a_separation.x :
		a_separation.z :
		a_separation.z < a_separation.y ?
		a_separation.z :
		a_separation.y;
}

bool CheckBoundingSpheres(float r1, glm::vec3 a_pos1, float r2, glm::vec3 a_pos2) {
	float distanceObjects = glm::distance(a_pos1, a_pos2);
	if (distanceObjects <= (r1 + r2)) 
		return true;
	
	return false;
}


bool CheckCollisionAABAAB(Shape *a_pShape1, Shape *a_pShape2,  std::list<Contact*> &a_contacts, ContactType a_type, bool generate)
{
	AABB* aabb1 = static_cast<AABB*>(a_pShape1);
	AABB* aabb2 = static_cast<AABB*>(a_pShape2);

	
	if (aabb1->mLeft > aabb2->mRight || aabb2->mLeft > aabb1->mRight) return false;
	if (aabb1->mBottom > aabb2->mTop || aabb2->mBottom > aabb1->mTop) return false;
	if (aabb1->mBack > aabb2->mFront || aabb2->mBack > aabb1->mFront) return false;
	if (generate) {

		glm::vec3
			deltaPosition = (a_pShape2->mCenter) - (a_pShape1->mCenter),// vector pos2 -> pos1
			r1extents = aabb1->mE,// distance from center to sides
			r2extents = aabb2->mE,// distance from center to sides
			separation = //distance among objects before contact
			
			glm::vec3(
				abs(deltaPosition.x) - r1extents.x - r2extents.x,
				abs(deltaPosition.y) - r1extents.y - r2extents.y,
				abs(deltaPosition.z) - r1extents.z - r2extents.z);


			Contact *pNewContact = new  Contact(a_pShape1->mpOwnerCollider, a_pShape2->mpOwnerCollider);
			pNewContact->mContactType = a_type;
			pNewContact->mNormal = CollisionNormal(separation, deltaPosition);//Which axis is the predominant for collision
			pNewContact->penetrationDepth = PenetrationDepth(separation); //to know how much they penetrate, takes the bigger axis difference
			a_contacts.push_back(pNewContact);
		}
		
		return true;

}

bool CheckCollisionSphereSphere(Shape *a_pShape1, Shape *a_pShape2, std::list<Contact*> &a_contacts, ContactType a_type, bool generate)
{
	SPHERE* c1 = static_cast<SPHERE*>(a_pShape1);
	SPHERE* c2 = static_cast<SPHERE*>(a_pShape2);

	if (CheckBoundingSpheres(c1->mRadius, c1->mCenter, c2->mRadius, c2->mCenter)) {
		if (generate) {
			float distanceObjects = glm::distance(c1->mCenter, c2->mCenter);
			Contact *pNewContact = new  Contact(a_pShape1->mpOwnerCollider, a_pShape2->mpOwnerCollider);
			pNewContact->penetrationDepth = distanceObjects - (c1->mRadius + c2->mRadius);
			pNewContact->mNormal = glm::normalize(c1->mCenter - c2->mCenter);
			a_contacts.push_back(pNewContact);
		}
		
		return true;
	}
	return false;
};


bool CheckCollisionOBBOBB(Shape *a_pShape1,Shape *a_pShape2, std::list<Contact*> &a_contacts, ContactType a_type, bool generate) {
	OBB* obb1 = static_cast<OBB*>(a_pShape1);
	OBB* obb2 = static_cast<OBB*>(a_pShape2);
	{
		//do axis theorem here
		float r1, r2;
		glm::mat3 R, absR;
		obb1->GetHalfExtents();
		obb2->GetHalfExtents();
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				R[i][j] = glm::dot(obb1->mRot[i], obb2->mRot[j]);

		glm::vec3 t = obb2->mCenter  - obb1->mCenter ;
		t = glm::vec3(glm::dot(t, obb1->mRot[0]), glm::dot(t, obb1->mRot[1]), glm::dot(t, obb1->mRot[2]));

		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				absR[i][j] = fabsf(R[i][j]) + FLT_EPSILON;

		for (int i = 0; i < 3; ++i)
		{
			r1 = obb1->mE[i];
			r2 = obb2->mE[0] * absR[i][0] + obb2->mE[1] * absR[i][1] + obb2->mE[2] * absR[i][2];
			if (t[i] > r1 + r2) return false;
		}

		for (int i = 0; i < 3; ++i)
		{
			r1 = obb1->mE[0] * absR[0][i] + obb1->mE[1] * absR[1][i] + obb1->mE[2] * absR[2][i];
			r2 = obb2->mE[i];
			if (t[i] > r1 + r2) return false;
		}

		for (int i = 0; i < 3; ++i)
		{
			r1 = obb1->mE[0] * absR[0][i] + obb1->mE[1] * absR[1][i] + obb1->mE[2] * absR[2][i];
			r2 = obb2->mE[i];
			if (fabsf(t[0] * absR[0][i] + t[1] * absR[1][i] + t[2] * absR[2][i]) > r1 + r2) return false;
		}

		r1 = obb1->mE[1] * absR[2][0] + obb1->mE[2] * absR[1][0];
		r2 = obb2->mE[1] * absR[0][2] + obb2->mE[2] * absR[0][1];
		if (fabsf(t[2] * absR[1][0] - t[1] * absR[2][0]) > r1 + r2) return false;

		r1 = obb1->mE[1] * absR[2][1] + obb1->mE[2] * absR[1][1];
		r2 = obb2->mE[0] * absR[0][2] + obb2->mE[2] * absR[0][0];
		if (fabsf(t[2] * absR[1][1] - t[1] * absR[2][1]) > r1 + r2) return false;

		r1 = obb1->mE[1] * absR[2][2] + obb1->mE[2] * absR[1][2];
		r2 = obb2->mE[0] * absR[0][1] + obb2->mE[1] * absR[0][0];
		if (fabsf(t[2] * absR[1][2] - t[1] * absR[1][1]) > r1 + r2) return false;

		r1 = obb1->mE[0] * absR[2][0] + obb1->mE[2] * absR[0][0];
		r2 = obb2->mE[1] * absR[1][2] + obb2->mE[2] * absR[1][1];
		if (fabsf(t[0] * absR[2][0] - t[2] * absR[0][0]) > r1 + r2) return false;

		// -
		r1 = obb1->mE[0] * absR[2][1] + obb1->mE[2] * absR[1][0];
		r2 = obb2->mE[0] * absR[1][2] + obb2->mE[2] * absR[0][1];
		if (fabsf(t[0] * absR[2][1] - t[2] * absR[0][1]) > r1 + r2) return false;

		r1 = obb1->mE[0] * absR[2][2] + obb1->mE[2] * absR[0][2];
		r2 = obb2->mE[0] * absR[1][1] + obb2->mE[1] * absR[1][0];
		if (fabsf(t[0] * absR[2][2] - t[2] * absR[0][2]) > r1 + r2) return false;

		r1 = obb1->mE[0] * absR[1][0] + obb1->mE[2] * absR[0][0];
		r2 = obb2->mE[1] * absR[2][2] + obb2->mE[1] * absR[2][1];
		if (fabsf(t[1] * absR[0][0] - t[0] * absR[1][0]) > r1 + r2) return false;
		//----
		r1 = obb1->mE[0] * absR[1][1] + obb1->mE[1] * absR[0][1];
		r2 = obb2->mE[0] * absR[2][2] + obb2->mE[2] * absR[2][0];
		if (fabsf(t[1] * absR[0][1] - t[0] * absR[1][1]) > r1 + r2) return false;

		r1 = obb1->mE[0] * absR[1][2] + obb1->mE[1] * absR[0][2];
		r2 = obb2->mE[0] * absR[2][1] + obb2->mE[1] * absR[2][0];
		if (fabsf(t[1] * absR[0][2] - t[0] * absR[1][2]) > r1 + r2) return false;
		if (generate) {
			Contact *pNewContact = new Contact(obb1->mpOwnerCollider, obb2->mpOwnerCollider);
			float distanceObjects = glm::distance(obb1->mCenter , obb2->mCenter);
			pNewContact->penetrationDepth = distanceObjects - glm::length(obb1->mE) + glm::length(obb2->mE);
			pNewContact->mNormal = glm::normalize(obb1->mCenter  - obb2->mCenter);
			pNewContact->mContactType = a_type;
			a_contacts.push_back(pNewContact);
		}
		
		return true;
	}
	return false;
}

float SquareDistance1Axis(float a_posSphere, float a_cubeMin, float a_cubeMax) {
	return a_posSphere < a_cubeMin ? (a_cubeMin - a_posSphere) * (a_cubeMin - a_posSphere) :
		a_posSphere > a_cubeMax ? (a_posSphere - a_cubeMax) * (a_posSphere - a_cubeMax) :
		0;

}

bool CheckCollisionAABBSphere(Shape *a_pCube, Shape *a_pSphere, std::list<Contact*> &a_contacts, ContactType a_type, bool generate)
{
	///https://studiofreya.com/3d-math-and-physics/sphere-vs-aabb-collision-detection-test/
	AABB* cube = static_cast<AABB*>(a_pCube);
	SPHERE* sphere = static_cast<SPHERE*>(a_pSphere);

	// MIN & MAX on AABB
	glm::vec3 max = a_pCube->mCenter + glm::vec3(cube->mRight, cube->mTop, cube->mFront);
	glm::vec3 min = a_pCube->mCenter - glm::vec3(cube->mLeft, cube->mBottom, cube->mBack);

	//Obtain Distance from (nearest point cube-sphere) to sphere center 
	float squaredDistance =
		SquareDistance1Axis(a_pSphere->mCenter.x, min.x, max.x) +
		SquareDistance1Axis(a_pSphere->mCenter.y, min.y, max.y) +
		SquareDistance1Axis(a_pSphere->mCenter.z, min.z, max.z);


	//glm::vec3 

	//if is less OR EQUAL than the radius: collision
	if (squaredDistance <= (sphere->mRadius * sphere->mRadius))
	{
		glm::vec3 deltaPosition = a_pSphere->mCenter - a_pCube->mCenter;// vector SPhere -> Cube
		glm::vec3 cubeExtents = cube->GetHalfExtents();// dimentions of the cube

		//clamping center distance gives us the vector to the nearest side of the AABB
		//Closest point to AABB
		glm::vec3 closestPointAABB = glm::vec3(

			deltaPosition.x > cubeExtents.x ? cubeExtents.x :
			deltaPosition.x < -cubeExtents.x ? -cubeExtents.x :
			deltaPosition.x,

			deltaPosition.y > cubeExtents.y ? cubeExtents.y :
			deltaPosition.y < -cubeExtents.y ? -cubeExtents.y :
			deltaPosition.y,

			deltaPosition.z > cubeExtents.z ? cubeExtents.z :
			deltaPosition.z < -cubeExtents.z ? -cubeExtents.z :
			deltaPosition.z
		);

		// vector to closest point on AABB
		glm::vec3 vclosest = closestPointAABB - cube->mCenter;

		// vector from circle center to closest point
		glm::vec3 difference = vclosest - sphere->mCenter;

		//TODO: CALCULATE colissionNormal AND penetrationDepth for the contact
		if (generate) {
			Contact *pNewContact = new  Contact(a_pCube->mpOwnerCollider, a_pSphere->mpOwnerCollider);
			pNewContact->penetrationDepth = std::max(difference.z, std::max(difference.x, difference.y));
			
			a_contacts.push_back(pNewContact);
		}

		return true;
	}
	return false;
};
bool CheckCollisionSphereAABB(Shape *a_pSphere, Shape *a_pCube,  std::list<Contact*> &a_contacts, ContactType a_type, bool generate) {
	return CheckCollisionAABBSphere(a_pCube, a_pSphere, a_contacts, a_type,generate);
}

bool CheckCollisionOBBAABB(Shape *a_pObb,Shape *a_pAAbb, std::list<Contact*> &a_contacts, ContactType a_type, bool generate) {
	OBB* obb = static_cast<OBB*>(a_pObb);
	AABB* cube = static_cast<AABB*>(a_pAAbb);
	obb->GetHalfExtents();
	cube->GetHalfExtents();
	if (CheckBoundingSpheres(//bounding spheres check to prevent costly math
		glm::length(obb->mE),a_pObb->mCenter,
		glm::length(glm::vec3(cube->mRight, cube->mTop, cube->mFront)), cube->mCenter))
	{
		float r1, r2;
		glm::vec3 mU2[3];
		glm::vec3 mE2 = glm::vec3(cube->mRight, cube->mTop, cube->mFront);
		mU2[0] = glm::vec3(1, 0, 0);
		mU2[1] = glm::vec3(0, 1, 0);
		mU2[0] = glm::vec3(0, 0, 1);
		glm::mat3 R, absR;
		obb->GetHalfExtents();
		cube->GetHalfExtents();
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				R[i][j] = glm::dot(obb->mRot[i], mU2[j]);

		glm::vec3 t = (cube->mCenter) - (obb->mCenter);
		t = glm::vec3(glm::dot(t, obb->mRot[0]), glm::dot(t, obb->mRot[1]), glm::dot(t, obb->mRot[2]));

		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				absR[i][j] = fabsf(R[i][j]) + FLT_EPSILON;

		for (int i = 0; i < 3; ++i)
		{
			r1 = obb->mE[i];
			r2 = mE2[0] * absR[i][0] + mE2[1] * absR[i][1] + mE2[2] * absR[i][2];
			if (t[i] > r1 + r2) return false;
		}

		for (int i = 0; i < 3; ++i)
		{
			r1 = obb->mE[0] * absR[0][i] + obb->mE[1] * absR[1][i] + obb->mE[2] * absR[2][i];
			r2 = mE2[i];
			if (t[i] > r1 + r2) return false;
		}

		for (int i = 0; i < 3; ++i)
		{
			r1 = obb->mE[0] * absR[0][i] + obb->mE[1] * absR[1][i] + obb->mE[2] * absR[2][i];
			r2 = mE2[i];
			if (fabsf(t[0] * absR[0][i] + t[1] * absR[1][i] + t[2] * absR[2][i]) > r1 + r2) return false;
		}

		r1 = obb->mE[1] * absR[2][0] + obb->mE[2] * absR[1][0];
		r2 = mE2[1] * absR[0][2] + mE2[2] * absR[0][1];
		if (fabsf(t[2] * absR[1][0] - t[1] * absR[2][0]) > r1 + r2) return false;

		r1 = obb->mE[1] * absR[2][1] + obb->mE[2] * absR[1][1];
		r2 = mE2[0] * absR[0][2] + mE2[2] * absR[0][0];
		if (fabsf(t[2] * absR[1][1] - t[1] * absR[2][1]) > r1 + r2) return false;

		r1 = obb->mE[1] * absR[2][2] + obb->mE[2] * absR[1][2];
		r2 = mE2[0] * absR[0][1] + mE2[1] * absR[0][0];
		if (fabsf(t[2] * absR[1][2] - t[1] * absR[1][1]) > r1 + r2) return false;

		r1 = obb->mE[0] * absR[2][0] + obb->mE[2] * absR[0][0];
		r2 = mE2[1] * absR[1][2] + mE2[2] * absR[1][1];
		if (fabsf(t[0] * absR[2][0] - t[2] * absR[0][0]) > r1 + r2) return false;

		// -
		r1 = obb->mE[0] * absR[2][1] + obb->mE[2] * absR[1][0];
		r2 = mE2[0] * absR[1][2] + mE2[2] * absR[0][1];
		if (fabsf(t[0] * absR[2][1] - t[2] * absR[0][1]) > r1 + r2) return false;

		r1 = obb->mE[0] * absR[2][2] + obb->mE[2] * absR[0][2];
		r2 = mE2[0] * absR[1][1] + mE2[1] * absR[1][0];
		if (fabsf(t[0] * absR[2][2] - t[2] * absR[0][2]) > r1 + r2) return false;

		r1 = obb->mE[0] * absR[1][0] + obb->mE[2] * absR[0][0];
		r2 = mE2[1] * absR[2][2] + mE2[1] * absR[2][1];
		if (fabsf(t[1] * absR[0][0] - t[0] * absR[1][0]) > r1 + r2) return false;
		//----
		r1 = obb->mE[0] * absR[1][1] + obb->mE[1] * absR[0][1];
		r2 = mE2[0] * absR[2][2] + mE2[2] * absR[2][0];
		if (fabsf(t[1] * absR[0][1] - t[0] * absR[1][1]) > r1 + r2) return false;

		r1 = obb->mE[0] * absR[1][2] + obb->mE[1] * absR[0][2];
		r2 = mE2[0] * absR[2][1] + mE2[1] * absR[2][0];
		if (fabsf(t[1] * absR[0][2] - t[0] * absR[1][2]) > r1 + r2) return false;
		if (generate) {
			Contact *pNewContact = new  Contact(obb->mpOwnerCollider, cube->mpOwnerCollider);
			float distanceObjects = glm::distance(obb->mCenter, cube->mCenter);
			pNewContact->penetrationDepth = distanceObjects - glm::length(obb->mE) + glm::length(glm::vec3(cube->mRight, cube->mTop, cube->mFront));
			pNewContact->mNormal = glm::normalize(obb->mCenter - cube->mCenter);
			pNewContact->mContactType = a_type;
			a_contacts.push_back(pNewContact);
		}
		
		return true;
	}

	return false;
}

bool CheckCollisionAABBOBB(Shape *a_pAAbb,  Shape *a_pObb, std::list<Contact*> &a_contacts, ContactType a_type, bool generate) {
	return CheckCollisionOBBAABB(a_pObb, a_pAAbb, a_contacts, a_type,generate);
}

bool CheckCollisionOBBSphere(Shape *a_pObb, Shape *a_pSphere, std::list<Contact*> &a_contacts, ContactType a_type, bool generate) {
	///https://studiofreya.com/3d-math-and-physics/sphere-vs-aabb-collision-detection-test
	OBB* obb = static_cast<OBB*>(a_pObb);
	obb->GetHalfExtents();
	SPHERE* sphere = static_cast<SPHERE*>(a_pSphere);

	if (!CheckBoundingSpheres(//bounding spheres check to prevent costly math
		glm::length(obb->mE), obb->mCenter,
		sphere->mRadius, sphere->mCenter))
		return false;



	glm::mat3 rots = glm::mat3();
	rots[0][0] = obb->mRot[0][0];
	rots[0][1] = obb->mRot[0][1];
	rots[0][2] = obb->mRot[0][2];

	rots[1][0] = obb->mRot[1][0];
	rots[1][1] = obb->mRot[1][1];
	rots[1][2] = obb->mRot[1][2];

	rots[2][0] = obb->mRot[2][0];
	rots[2][1] = obb->mRot[2][1];
	rots[2][2] = obb->mRot[2][2];

	glm::vec3 relSpherePos = rots * sphere->mCenter;
	glm::vec3 relObbPos = rots * obb->mCenter;

	// MIN & MAX on AABB
	glm::vec3 max = relObbPos + glm::vec3(obb->mE[0], obb->mE[1], obb->mE[2]);
	glm::vec3 min = relObbPos - glm::vec3(obb->mE[0], obb->mE[1], obb->mE[2]);

	//Obtain Distance from (nearest point cube-sphere) to sphere center 
	float squaredDistance =
		SquareDistance1Axis(relSpherePos.x, min.x, max.x) +
		SquareDistance1Axis(relSpherePos.y, min.y, max.y) +
		SquareDistance1Axis(relSpherePos.z, min.z, max.z);


	//glm::vec3 

	//if is less OR EQUAL than the radius: collision
	if (squaredDistance <= (sphere->mRadius * sphere->mRadius))
	{
		glm::vec3 deltaPosition = relSpherePos - relObbPos;// vector SPhere -> Cube
		glm::vec3 cubeExtents = rots * obb->GetHalfExtents();// dimentions of the cube

		//clamping center distance gives us the vector to the nearest side of the AABB
		//Closest point to AABB
		glm::vec3 closestPointOBB = glm::vec3(

			deltaPosition.x > cubeExtents.x ? cubeExtents.x :
			deltaPosition.x < -cubeExtents.x ? -cubeExtents.x :
			deltaPosition.x,

			deltaPosition.y > cubeExtents.y ? cubeExtents.y :
			deltaPosition.y < -cubeExtents.y ? -cubeExtents.y :
			deltaPosition.y,

			deltaPosition.z > cubeExtents.z ? cubeExtents.z :
			deltaPosition.z < -cubeExtents.z ? -cubeExtents.z :
			deltaPosition.z
		);
		if (generate) {
			glm::mat3 inverseRot = glm::transpose(rots);

			closestPointOBB = inverseRot * closestPointOBB;

			// vector to closest point on AABB
			glm::vec3 vclosest = closestPointOBB - obb->mCenter;

			// vector from circle center to closest point
			glm::vec3 difference = vclosest - sphere->mCenter;

			Contact *pNewContact = new Contact(a_pObb->mpOwnerCollider, a_pSphere->mpOwnerCollider);
			float distanceObjects = glm::distance(obb->mCenter, sphere->mCenter);
			pNewContact->penetrationDepth = glm::length(difference);
			pNewContact->mNormal = glm::normalize(difference);
			a_contacts.push_back(pNewContact);
		}
		return true;
	}
	return false;
}

bool CheckCollisionSphereOBB(Shape *a_pSphere, Shape *a_pObb, std::list<Contact*> &a_contacts, ContactType a_type, bool generate) {
	return CheckCollisionOBBSphere(a_pObb, a_pSphere, a_contacts, a_type,generate);
}



CollisionManager::CollisionManager()
{
	// set all function pointers
	CollisionFunctions[_AABB][_AABB] = CheckCollisionAABAAB;
	CollisionFunctions[_SPHERE][_SPHERE] = CheckCollisionSphereSphere;
	CollisionFunctions[_OBB][_OBB] = CheckCollisionOBBOBB;

	CollisionFunctions[_AABB][_SPHERE] = CheckCollisionAABBSphere;
	CollisionFunctions[_SPHERE][_AABB] = CheckCollisionSphereAABB;

	CollisionFunctions[_AABB][_OBB] = CheckCollisionAABBOBB;
	CollisionFunctions[_OBB][_AABB] = CheckCollisionOBBAABB;

	CollisionFunctions[_OBB][_SPHERE] = CheckCollisionOBBSphere;
	CollisionFunctions[_SPHERE][_OBB] = CheckCollisionSphereOBB;


}

CollisionManager::~CollisionManager()
{
}

void CollisionManager::ResetContacts()
{
	for (Contact * contact : mPrevContacts)//erases list of prevcontacts
	{
		contact->mpColliderMain->mIsCollided = false;
		contact->mpColliderOther->mIsCollided = false;
		delete contact;
	}
	mPrevContacts.clear();

	for (Contact * contact : mActualContacts)//makes a list of prevContacts, 
		mPrevContacts.push_back(contact);

	mActualContacts.clear();//erases contacts

	mContacts.clear();
}

void CollisionManager::CheckContact(Collider *a_pCol1, Collider* a_pCol2)
{
	if (CheckCompatibility(a_pCol1->mLType, a_pCol2->mLType))// if these colliders are ment to be colliding by type V.G: Bullet->Enemy
	{
		ContactType _ContactType =
			(a_pCol1->mIsTrigger && !a_pCol2->mIsTrigger) || (!a_pCol1->mIsTrigger && a_pCol2->mIsTrigger) ? ContactType::OnTriggerEnter :
			(!a_pCol1->mIsTrigger && !a_pCol2->mIsTrigger) ? ContactType::OnCollisionEnter :
			ContactType::NonCollision;
		if (_ContactType != ContactType::NonCollision)
		{
			//use GJK and EPA HERE
			//*
			Contact* contact = GJK::CheckContact(a_pCol1, a_pCol2, _ContactType);
			if (nullptr != contact) 
			{
				mActualContacts.push_back(contact);
				a_pCol1->mIsCollided = true;
				a_pCol2->mIsCollided = true;
			}
			//*/
			/*
			if (CheckCollisionAndGenerateContact(a_pCol1->mpShape,a_pCol2->mpShape,_ContactType)) {
				a_pCol1->mIsCollided = true;
				a_pCol2->mIsCollided = true;
			}
			//*/
		}
	}
}

void CollisionManager::SolveContactsReactions()
{
	for (auto contact : mContacts) {
		// Generate Contact Event
		ContactEvent* pContactEvent = static_cast<ContactEvent*>(MemoryManager::GetInstance()->GetEventFromMemory("ContactEvent"));
		pContactEvent->mId1 = contact->mpBodyMain->GetOwnerID();
		pContactEvent->mId2 = contact->mpBodyOther->GetOwnerID();
		pContactEvent->mContactNormal = std::tuple<float, float, float>(contact->mNormal.x, contact->mNormal.y, contact->mNormal.z);
		pContactEvent->mContactType = contact->mContactType;

		// Send Contact Event
		EventManager::GetInstance()->SendEvent(pContactEvent, pContactEvent->mId1);
		EventManager::GetInstance()->SendEvent(pContactEvent, pContactEvent->mId2);
	}
}

bool Contact::isTrigger() {
	if (mContactType == ContactType::OnTriggerEnter || mContactType == ContactType::OnTriggerStay || mContactType == ContactType::OnTriggerExit)
		return true;
	return false;
}

GEAR_API glm::vec3 Contact::GetRelativeVelocity(Collider * col)
{
	glm::vec3 result = glm::vec3(0);
	Body* body = col->mpBody;
	glm::vec3 pos = col->mpShape->mCenter;
	glm::vec3 rPos = colPoint - pos;

	result = body->mVel + glm::cross(body->mOmega, rPos);
	return result;
}

void CollisionManager::ApplyImpulses(float dt)
{
	float partial_dt = dt / impulseIterations;
	for (int i = 0; i < impulseIterations; ++i)
		for (auto contact : mContacts)
			if (!contact->isTrigger())
				contact->ApplyImpulse(partial_dt);
}

void CollisionManager::ConstrainVelocity()
{
	for (auto contact : mContacts) {
		contact->mpBodyMain->ConstrainVelocity();
		contact->mpBodyOther->ConstrainVelocity();
	}

}

void CollisionManager::CorrectPenetration()
{
	for (auto c : mContacts)
		if (!c->mpColliderMain->mIsTrigger && !c->mpColliderOther->mIsTrigger)
			c->CorrectPenetration();
}

bool CollisionManager::Colliding(std::string a_first, std::string a_second) {
	bool _bothPresent = (mType0 == a_first && mType1 == a_second) || (mType1 == a_first && mType0 == a_second) ? true : false;
	if (_bothPresent)
		mInOrder = mType0 == a_first && mType1 == a_second ? true : false;
	return _bothPresent;
}

bool CollisionManager::Colliding(std::string a_first, std::string a_second, std::string a_type0, std::string a_type1) {
	return  (a_type0 == a_first && a_type1 == a_second) || (a_type1 == a_first && a_type0 == a_second) ? true : false;
}

bool CollisionManager::CheckCompatibility(std::string a_type1, std::string a_type2)
{

	if (Colliding("Player", "Box", a_type1, a_type2))
		return true;
	if (Colliding("Player", "Platform", a_type1, a_type2))
		return true;
	if (Colliding("Player-foot", "Box", a_type1, a_type2))
		return true;
	if (Colliding("MovingBox", "Box", a_type1, a_type2))
		return true;
	if (Colliding("MovingBox", "Player", a_type1, a_type2))
		return true;
	if (Colliding("MovingBox", "Player-foot", a_type1, a_type2))
		return true;
	if (Colliding("Attack", "Player", a_type1, a_type2))
		return true;
	if (Colliding("Attack", "Player-foot", a_type1, a_type2))
		return true;
	if (Colliding("MovingBox", "MovingBox", a_type1, a_type2))
		return true;
	if (Colliding("SearchArea", "Container", a_type1, a_type2))
		return true;
	if (Colliding("SearchArea", "GridCell", a_type1, a_type2))
		return true;
	if (Colliding("SearchArea", "Processor", a_type1, a_type2))
		return true;
	if (Colliding("SearchArea", "Generator", a_type1, a_type2))
		return true;
	if (Colliding("SearchArea", "Resource", a_type1, a_type2))
		return true;
	if (Colliding("SearchArea", "Block", a_type1, a_type2))
		return true;
	if (Colliding("Resource", "Box", a_type1, a_type2))
		return true;
	if (Colliding("Resource", "Resource", a_type1, a_type2))
		return true;
	if (Colliding("Resource", "Player", a_type1, a_type2))
		return true;
	if (Colliding("Resource", "Platform", a_type1, a_type2))
		return true;
	if (Colliding("Player", "Player", a_type1, a_type2))
		return true;
	if (Colliding("Processor", "Player", a_type1, a_type2))
		return true;
	if (Colliding("Processor", "Resource", a_type1, a_type2))
		return true;
	if (Colliding("Container", "Player", a_type1, a_type2))
		return true;
	if (Colliding("Container", "Resource", a_type1, a_type2))
		return true;
	if (Colliding("Generator", "Player", a_type1, a_type2))
		return true;
	if (Colliding("Generator", "Resource", a_type1, a_type2))
		return true;
	if (Colliding("Block", "Player", a_type1, a_type2))
		return true;
	if (Colliding("Block", "Resource", a_type1, a_type2))
		return true;

	if (Colliding("BULLET", "Enemy", a_type1, a_type2))
		return true;
	if (Colliding("BULLET", "Player", a_type1, a_type2))
		return true;
	if (Colliding("BULLET", "Wall", a_type1, a_type2))
		return true;
	if (Colliding("Player", "LevelChange", a_type1, a_type2))
		return true;

	if (Colliding("BULLET", "Box", a_type1, a_type2))
		return true;
	if (Colliding("Grenade", "Box", a_type1, a_type2))
		return true;
	if (Colliding("Grenade", "Enemy", a_type1, a_type2))
		return true;

	if (Colliding("Ball", "Goal", a_type1, a_type2))
		return true;
	if (Colliding("Ball", "Side", a_type1, a_type2))
		return true;
	if (Colliding("Ball", "Player", a_type1, a_type2))
		return true;
	if (Colliding("Ball", "Attack", a_type1, a_type2))
		return true;
	if (Colliding("Ball", "Box", a_type1, a_type2))
		return true;
	if (Colliding("Teleport", "Player", a_type1, a_type2))
		return true;
	if (Colliding("LevelButton", "Player", a_type1, a_type2))
		return true;

	return false;// not Match Known, Handle it.
}

bool CollisionManager::CheckCollisionAndGenerateContact(Shape * a_pShape1, Shape * a_pShape2, ContactType a_type)
{
	return CollisionFunctions[a_pShape1->mType][a_pShape2->mType](a_pShape1, a_pShape2, mActualContacts,a_type,true);
}
GEAR_API bool CollisionManager::CheckCollision(Shape * pShape1, Shape * pShape2,ContactType _type)
{
	return CollisionFunctions[pShape1->mType][pShape2->mType](pShape1, pShape2, mActualContacts, _type,false);
}

bool CollisionManager::OnCollisionEnter(Collider * a_collider)
{
	return	CheckCollision(a_collider, ContactType::OnCollisionEnter);
}
bool CollisionManager::OnCollisionExit(Collider * a_collider)
{
	return	CheckCollision(a_collider, ContactType::OnCollisionExit);
}
bool CollisionManager::OnCollisionStay(Collider * a_collider)
{
	return CheckCollision(a_collider, ContactType::OnCollisionExit);
}
bool CollisionManager::OnTriggerEnter(Collider * a_collider)
{
	return	CheckCollision(a_collider, ContactType::OnTriggerEnter);
}
bool CollisionManager::OnTriggerExit(Collider * a_collider)
{
	return	CheckCollision(a_collider, ContactType::OnTriggerExit);
}
bool CollisionManager::OnTriggerStay(Collider * a_collider)
{
	return	CheckCollision(a_collider, ContactType::OnTriggerStay);
}


bool CollisionManager::OnCollisionEnter(Collider * a_collider1, Collider * a_collider2)
{
	return	CheckCollision(a_collider1, a_collider2, ContactType::OnCollisionEnter);
}
bool CollisionManager::OnCollisionExit(Collider * a_collider1, Collider * a_collider2)
{
	return	CheckCollision(a_collider1, a_collider2, ContactType::OnCollisionExit);
}
bool CollisionManager::OnCollisionStay(Collider * a_collider1, Collider * a_collider2)
{
	return CheckCollision(a_collider1, a_collider2, ContactType::OnCollisionExit);
}
bool CollisionManager::OnTriggerEnter(Collider * a_collider1, Collider * a_collider2)
{
	return	CheckCollision(a_collider1, a_collider2, ContactType::OnTriggerEnter);
}
bool CollisionManager::OnTriggerExit(Collider * a_collider1, Collider * a_collider2)
{
	return	CheckCollision(a_collider1, a_collider2, ContactType::OnTriggerExit);
}
bool CollisionManager::OnTriggerStay(Collider * a_collider1, Collider * a_collider2)
{
	return	CheckCollision(a_collider1, a_collider2, ContactType::OnTriggerStay);
}