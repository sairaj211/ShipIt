#pragma once

#include "Event.h"
/*!
 * \brief event to tell components to pause or unpause
 * 
 */
class  PauseEvent :
	public Event
{
public:
	/*!
	 * Default Ctor
	 * 
	 */
	GEAR_API PauseEvent();
	/*!
	 * Default Dtor
	 * 
	 */
	GEAR_API ~PauseEvent();
	GEAR_API virtual PauseEvent* Create() { return new PauseEvent(); };
public:
	bool mPause;
	int mLayer;
};

