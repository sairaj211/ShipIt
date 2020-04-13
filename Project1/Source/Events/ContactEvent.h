#pragma once
#include "Event.h"
#include "Physics/PhysicsManager.h"
class Body;
class ContactEvent : public Event
{
public:
	GEAR_API ContactEvent();
	GEAR_API ~ContactEvent();
	GEAR_API virtual ContactEvent* Create() { return new ContactEvent(); };
public:
	int mId1;/*!< One of the contact Ids */
	int mId2;/*!< Other contact Id */
	std::tuple<float, float, float> mContactPoint;/*!< Point of contact in tuple form for python usage */
	std::tuple<float, float, float> mContactNormal;/*!< Normal of contact in tuple form for python usage */
	ContactType mContactType; /*!< Type of Contact used */
};

