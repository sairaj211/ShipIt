#pragma once
#include "Event.h"
class AttackEvent : public Event
{
public:
	GEAR_API AttackEvent();
	GEAR_API ~AttackEvent();
	GEAR_API virtual void Reset();
	GEAR_API virtual AttackEvent* Create() { return new AttackEvent(); };
public:
	int mAttackId; /*!< ID of the Entity that is the attack */
	float mForceAmount = 5.f; /*!< the force amount of the attack*/
};

