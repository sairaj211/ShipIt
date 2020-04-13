#pragma once
#include "Event.h"

/*!
 * An event for when a Request is made
 *
 */
class RequestMadeEvent : public Event
{
public:
	/*!
	 * RequestMadeEvent Default Constructor
	 *
	 */
	GEAR_API RequestMadeEvent();
	/*!
	 * RequestMadeEvent Default Destructor
	 *
	 */
	GEAR_API ~RequestMadeEvent();
	/*!
	 * Resets the RequestMadeEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new RequestMadeEvent
	 *
	 * \return A pointer to a new RequestMadeEvent
	 */
	GEAR_API virtual RequestMadeEvent* Create();

public:
	int mGame; /*!< Integer representation of the game being requested */
};

