#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "PhysicsManager.h"
#include "Engine.h"
#include "Events/ContactEvent.h"
#include "Managers/MemoryManager.h"
#include "Timer.h"
#include "Managers/EventManager.h"
PhysicsManager* PhysicsManager::mInstance = nullptr;

#define COMPONENT_MANAGER ComponentManager::GetInstance()

//#define THREADING 1
#define NOTHREADING 1




bool PhysicsManager::OnCollisionEnter(Collider * a_collider)
{
	return	mpCollisionManager->CheckCollision(a_collider, ContactType::OnCollisionStay);
}
bool PhysicsManager::OnCollisionExit(Collider * a_collider)
{
	return	mpCollisionManager->CheckCollision(a_collider, ContactType::OnCollisionExit);
}
bool PhysicsManager::OnCollisionStay(Collider * a_collider)
{
	return	mpCollisionManager->CheckCollision(a_collider, ContactType::OnCollisionStay);
}
bool PhysicsManager::OnTriggerEnter(Collider * a_collider)
{
	return	mpCollisionManager->CheckCollision(a_collider, ContactType::OnTriggerEnter);
}
bool PhysicsManager::OnTriggerExit(Collider * a_collider)
{
	return	mpCollisionManager->CheckCollision(a_collider, ContactType::OnTriggerExit);
}
bool PhysicsManager::OnTriggerStay(Collider * a_collider)
{
	return	mpCollisionManager->CheckCollision(a_collider, ContactType::OnTriggerStay);
}

bool PhysicsManager::OnCollisionEnter(Collider * a_collider1, Collider * a_collider2)
{
	return	mpCollisionManager->CheckCollision(a_collider1, a_collider2, ContactType::OnCollisionStay);
}
bool PhysicsManager::OnCollisionExit(Collider * a_collider1, Collider * a_collider2)
{
	return	mpCollisionManager->CheckCollision(a_collider1, a_collider2, ContactType::OnCollisionExit);
}
bool PhysicsManager::OnCollisionStay(Collider * a_collider1, Collider * a_collider2)
{
	return	mpCollisionManager->CheckCollision(a_collider1, a_collider2, ContactType::OnCollisionStay);
}
bool PhysicsManager::OnTriggerEnter(Collider * a_collider1, Collider * a_collider2)
{
	
	return	mpCollisionManager->CheckCollision(a_collider1, a_collider2, ContactType::OnTriggerEnter);
}
bool PhysicsManager::OnTriggerExit(Collider * a_collider1, Collider * a_collider2)
{
	return	mpCollisionManager->CheckCollision(a_collider1, a_collider2, ContactType::OnTriggerExit);
}
bool PhysicsManager::OnTriggerStay(Collider * a_collider1, Collider * a_collider2)
{
	return	mpCollisionManager->CheckCollision(a_collider1, a_collider2, ContactType::OnTriggerStay);
}

bool CollisionManager::CheckCollision(Collider * a_collider, ContactType a_contactType)
{
	for (auto contact : mContacts)
		if (contact->mpColliderMain == a_collider || contact->mpColliderOther == a_collider)
			if (contact->mContactType == a_contactType)
				return true;
	return false;
}
//TOOD this search must not be a FOR. use STL power.
bool CollisionManager::CheckCollision(Collider * a_collider1, Collider * a_collider2, ContactType a_contactType)
{
	for (auto contact : mContacts)
		if ((contact->mpColliderMain == a_collider1 && contact->mpColliderOther == a_collider2) ||
			(contact->mpColliderMain == a_collider2 && contact->mpColliderOther == a_collider1))
			if (contact->mContactType == a_contactType)
				return true;
	return false;
}



Collider * CollisionManager::RetrieveFirstContact(Collider * a_Collider, std::string a_mlType, ContactType a_contactType)
{
	for (auto contact : mContacts)
	{
		if (contact->mpColliderMain == a_Collider && contact->mpColliderOther->mLType == a_mlType && contact->mContactType == a_contactType)
		{
			return contact->mpColliderOther;
		}
		if (contact->mpColliderOther == a_Collider && contact->mpColliderMain->mLType == a_mlType && contact->mContactType == a_contactType)
		{
			return contact->mpColliderMain;
		}
	}
	return nullptr;
}

PhysicsManager::PhysicsManager()
{
	gravity = glm::vec3(0.0, -9.81f, 0.0);
	mpCollisionManager = new CollisionManager();
	dynamicAABBTree = new AABBTree();
}

void PhysicsManager::Init()
{
	LOG_INFO("PhysicsManager Init");
	std::vector<Component*> trColliderList = COMPONENT_MANAGER->GetComponents("Collider");
	dynamicAABBTree->BuildTree(trColliderList);
	//trColliderList = nullptr;
	/*isRunning[0] = true;
	isRunning[1] = true;

	threadFlag[0] = false;
	threadFlag[1] = false;

	workerThread[0] = new std::thread(std::bind(&PhysicsManager::Thread1,this, 0));
	workerThread[1] = new std::thread(std::bind(&PhysicsManager::Thread1, this, 1));

 	isRunning[2] = true;
 	isRunning[3] = true;
 
 	threadFlag[2] = false;
 	threadFlag[3] = false;
 
 	workerThread[2] = new std::thread(std::bind(&PhysicsManager::Thread2, this, 2));
 	workerThread[3] = new std::thread(std::bind(&PhysicsManager::Thread2, this, 3));*/
 }

PhysicsManager::~PhysicsManager()
{
	delete mpCollisionManager;
	//delete dynamicAABBTree;
	/*isRunning[0] = false;
	isRunning[1] = false;

	threadFlag[0] = false;
	threadFlag[1] = false;

	workerThread[0]->join();
	workerThread[1]->join();



	isRunning[2] = false;
	isRunning[3] = false;

	threadFlag[2] = false;
	threadFlag[3] = false;

	workerThread[2]->join();
	workerThread[3]->join();

	std::unique_lock<std::shared_mutex> lock{ m_mutex };*/

}

void PhysicsManager::ResetForces()
{
	std::vector<Component*> trBodyList = COMPONENT_MANAGER->GetComponents("Body");
	for (Component* pComponent : trBodyList)
	{
		Body *pBody = static_cast<Body*>(pComponent);
		pBody->mVel = glm::vec3(0, 0, 0);
		pBody->mTorque = glm::vec3(0, 0, 0);
	}
}

PhysicsManager * PhysicsManager::GetInstance()
{
	if (!mInstance) {
		mInstance = new PhysicsManager();
	}
	return mInstance;
}

void PhysicsManager::Cleanup()
{
	delete mInstance;
}

GEAR_API CollisionManager * PhysicsManager::GetCollisionManager()
{
	return mpCollisionManager;
}

void PhysicsManager::Update(float a_fixedDeltaTime)
{

#ifdef NOTHREADING
	//Timer time("Update Loop");
	fixedDeltaTime = 17.f * 0.001f;
	//fixedDeltaTime = .0017;
	//RESET CONTACTS
	mpCollisionManager->ResetContacts();

	//AABBTree update here

	// GENERATE CONTACTS
	//Timer time("Generate Contact Loop");

	std::vector<Component*> trColliderList = COMPONENT_MANAGER->GetComponents("Collider");
	//AABB Tree to generate list of potential contacts here
	dynamicAABBTree->Update(trColliderList);
	//BroadPhase
	std::list<std::pair<Collider*, Collider*>> pairs = dynamicAABBTree->BroadPhase(mpCollisionManager);
	for (auto pair : pairs) {
		//Narrow Phase
		mpCollisionManager->CheckContact(pair.first, pair.second);
	}

	//COLLISION/TRIGGER ENTER STAY EXIT
	mpCollisionManager->IdentifyContactTypes();

	//	Timer time("Update Accleration");
		// UPDATE ACCELERATION 

	std::vector<Component*> trBodyList = COMPONENT_MANAGER->GetComponents("Body");
	for (Component* pComponent : trBodyList) {
		Body *pBody = static_cast<Body*>(pComponent);
		pBody->UpdateGravityForces(fixedDeltaTime,gravity);
	}

	//Integrate for linear and angular velocities 
	for (Component* pComponent1 : trBodyList) {
		Body* pBody = static_cast<Body*>(pComponent1);
		if (pBody->mMass > 0.0f)
			pBody->Integrate(fixedDeltaTime);//Semi-Implict Euler done here
	}
	

	//APPLY VELOCITY  CONSTRAINS
	mpCollisionManager->ConstrainVelocity();
	// UPDATE IMPULSES
	mpCollisionManager->ApplyImpulses(fixedDeltaTime);

	//Integrate for linear and angular velocities post impulses
	for (Component* pComponent1 : trBodyList) {
		Body* pBody = static_cast<Body*>(pComponent1);
		if (pBody->mMass > 0.0f) {
			pBody->Integrate(fixedDeltaTime);//Semi-Implict Euler done here
			pBody->UpdatePositions(fixedDeltaTime);
		}
	}



	//CORRECT PENETRATION 
	mpCollisionManager->CorrectPenetration();

	

	//SOLVE CONTACT REACTIONS--------------------------------------------
	mpCollisionManager->SolveContactsReactions();

#endif


#ifdef THREADING
	//Timer time("Update Loop (thread)");
	fixedDeltaTime = a_fixedDeltaTime * 0.001f;

	//RESET CONTACTS
	mpCollisionManager->ResetContacts();

	// GENERATE CONTACTS

	threadFlag[0] = true;
	threadFlag[1] = true;
	while (threadFlag[0] || threadFlag[1])
	{
	}

	//COLLISION/TRIGGER ENTER STAY EXIT
	mpCollisionManager->IdentifyContactTypes();


	//	Timer time("Update Accleration");
		// UPDATE ACCELERATION 
	std::vector<Component*> trBodyList = COMPONENT_MANAGER->GetComponents("Body");
	for (Component* pComponent : trBodyList) {
		Body *pBody = static_cast<Body*>(pComponent);
		pBody->UpdateGravityForces(fixedDeltaTime, gravity);
	}

	// UPDATE IMPULSES
	mpCollisionManager->ApplyImpulse();

	//APPLY VELOCITY  CONSTRAINS
	mpCollisionManager->ConstrainVelocity();

	//UPDATE POSITION 
	for (Component* pComponent1 : trBodyList) {
		Body* pBody = static_cast<Body*>(pComponent1);
		if (pBody->mInvMass > 0.0f)
			pBody->UpdatePositions(fixedDeltaTime);
	}

	//UPDATE POSITION 
	threadFlag[2] = true;
	threadFlag[3] = true;
	while (threadFlag[2] || threadFlag[3])
	{
	}

	//CORRECT PENETRATION 
	mpCollisionManager->CorrectPenetration();

	mpCollisionManager->SolveContactsReactions();
#endif

}




void PhysicsManager::Thread1(size_t a_thread_ID)
{
	while (isRunning[a_thread_ID])
	{
		if (threadFlag[a_thread_ID])
		{
			std::vector<Component*> trColliderList = COMPONENT_MANAGER->GetComponents("Collider");
			size_t size = trColliderList.size();
			size_t midPoint = size / 2;
			size_t startLoop = a_thread_ID == 0 ? 0 : midPoint;
			size_t endLoop = a_thread_ID == 0 ? midPoint : size;
			for (size_t i = startLoop; i < endLoop; ++i)
			{
				GenerateContacts(trColliderList[i], trColliderList);
			}
			threadFlag[a_thread_ID] = false;
		}
	}
}

void PhysicsManager::GenerateContacts(Component * a_pComponent1,std::vector<Component*> a_trColliderList/*, float fixedDeltaTime*/)
{
	bool startWithIthself = false;// to avoid Duplicate Checking , and self collision
	for (Component* pComponent2 : a_trColliderList)
	{
		if (!startWithIthself) {// im supposing will trasverse the same order . cause is the same list
			if (a_pComponent1 == pComponent2) {
				startWithIthself = true;
				continue;
			}
			continue;//offset until your oun position on list
		}
		mpCollisionManager->CheckContact(static_cast<Collider*>(a_pComponent1), static_cast<Collider*>(pComponent2));
	}
}


void PhysicsManager::Thread2(size_t a_thread_ID)
{
	while (isRunning[a_thread_ID])
	{
		if (threadFlag[a_thread_ID])
		{
			std::vector<Component*> trBodyList = COMPONENT_MANAGER->GetComponents("Body");//
			size_t size = trBodyList.size();
			size_t midPoint = size / 2;
			size_t startLoop = a_thread_ID == 2 ? 0 : midPoint;
			size_t endLoop = a_thread_ID == 2 ? midPoint : size;

			for (size_t i = startLoop; i < endLoop; ++i)
			{
				updateAccleration1(trBodyList[i], fixedDeltaTime);
			}
			threadFlag[a_thread_ID] = false;
		}
	}
}

void PhysicsManager::updateAccleration1(Component * a_pComponent1, float a_fixedDeltaTime)
{
	Body* pBodyupdate = static_cast<Body*>(a_pComponent1);
	if (pBodyupdate->mInvMass > 0.0f)
		pBodyupdate->Integrate(a_fixedDeltaTime);
}

// raycast it retrieves 
//a collider
// a interception time .
std::pair<Collider *, glm::vec3 > PhysicsManager::Raycast(Ray * a_ray)
{
	std::pair< float, Collider*> minIntersection = std::pair(INFINITY, nullptr);//minIntersectionTime,colliderCandidate

	std::vector<Component*> trColliderList = COMPONENT_MANAGER->GetComponents("Collider");//obtain list of colliders
	for (Component* pComponent : trColliderList) {
		Collider * myCollider = static_cast<Collider*>(pComponent);//obtain collider
		if (myCollider->mIsTrigger)
			continue;// if is trigger, exclude 

		//Here all exclusion  conditions must be declared 


		AABB * myCube = static_cast<AABB*>(myCollider->mpShape);
		std::pair<bool, float>intersecting = myCube->Intersected(a_ray);//is Intercepting?, timeIntersection

		if (myCollider->mLType != "Player" && intersecting.first   &&    intersecting.second < minIntersection.first) {// if intersection, and time is lower than mine 
			minIntersection.first = intersecting.second;// the new min intersection time is lower than actual 
			minIntersection.second = myCollider;// this collider is the new candidate
		}
	}
	if (minIntersection.first != INFINITY) { //if found any Collider,
		glm::vec3 contactPoint = a_ray->origin + a_ray->direction * minIntersection.first;
		return std::make_pair(
			minIntersection.second,
			contactPoint);
	};
	return  std::make_pair(nullptr, glm::vec3(0));//false, and no reference to collider
}

GEAR_API AABBTree * PhysicsManager::GetTree()
{
	return dynamicAABBTree;
}

