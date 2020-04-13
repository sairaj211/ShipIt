#pragma once
#include "Event.h"

/*!
 * An event for when a level is initialized
 *
 */
class UpdateLevelEvent : public Event
{
public:
	/*!
	 * UpdateLevelEvent Default Constructor
	 *
	 */
	GEAR_API UpdateLevelEvent();
	/*!
	 * UpdateLevelEvent Default Destructor
	 *
	 */
	GEAR_API ~UpdateLevelEvent();
	/*!
	 * Resets the UpdateLevelEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new UpdateLevelEvent
	 *
	 * \return A pointer to a new UpdateLevelEvent
	 */
	GEAR_API virtual UpdateLevelEvent* Create();

public:
	float mCurrentTime; /*!< Time remaining for the player */
};

