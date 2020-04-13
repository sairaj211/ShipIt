#pragma once
#include "Event.h"

/*!
 * An event for when a level is initialized
 *
 */
class UpdateSliderPositionEvent : public Event
{
public:
	/*!
	 * UpdateLevelEvent Default Constructor
	 *
	 */
	GEAR_API UpdateSliderPositionEvent();
	/*!
	 * UpdateLevelEvent Default Destructor
	 *
	 */
	GEAR_API ~UpdateSliderPositionEvent();
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
	GEAR_API virtual UpdateSliderPositionEvent* Create();

public:
	glm::vec3 mSliderPosition; /*!< Time remaining for the player */
};

