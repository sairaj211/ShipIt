#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "AttackEvent.h"


AttackEvent::AttackEvent() :Event("AttackEvent")
{
}



AttackEvent::~AttackEvent()
{
}

GEAR_API void AttackEvent::Reset()
{
	mAttackId = 0;
}
