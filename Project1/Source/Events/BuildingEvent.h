#pragma once
#include "Event.h"
class BuildingEvent : public Event
{
public:
	GEAR_API BuildingEvent();
	GEAR_API ~BuildingEvent();
	GEAR_API virtual void Reset();
	GEAR_API virtual BuildingEvent* Create() { return new BuildingEvent(); };

};

