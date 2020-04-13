#pragma once
#include "Event.h"

/*!
 * An event for when an Entity's animation
 * state changes
 *
 */
class ChangeAnimationStateEvent : public Event
{
public:
	/*!
	 * ChangeAnimationStateEvent Default Constructor
	 *
	 */
	GEAR_API ChangeAnimationStateEvent();
	/*!
	 * ChangeAnimationStateEvent Default Destructor
	 *
	 */
	GEAR_API ~ChangeAnimationStateEvent();
	/*!
	 * Resets the ChangeAnimationStateEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new ChangeAnimationStateEvent
	 *
	 * \return A pointer to a new ChangeAnimationStateEvent
	 */
	GEAR_API virtual ChangeAnimationStateEvent* Create();

public:
	std::string mState; /*!< Name of the new state */
	int mID; /*!< ID of the Entity whose animation state was changed */
};