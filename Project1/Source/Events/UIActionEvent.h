#pragma once
#include "Event.h"

class UIActionEvent : public Event
{
public:
	GEAR_API UIActionEvent();
	GEAR_API ~UIActionEvent();
	GEAR_API void Reset() override;
	GEAR_API UIActionEvent* Create() override;

public:
	std::string mDescription;
	std::string mData;
	int mOwnerId;
};