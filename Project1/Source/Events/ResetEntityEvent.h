#pragma once
#include "Event.h"
class ResetEntityEvent :
	public Event
{
public:
	GEAR_API ResetEntityEvent();
	GEAR_API ~ResetEntityEvent();
	GEAR_API virtual ResetEntityEvent* Create() { return new ResetEntityEvent(); };
public:
	int mObjectId;
	float mResetX;
	float mResetY;
	float mResetZ;
};

